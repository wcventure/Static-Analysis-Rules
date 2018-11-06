static int dissect_spdy_data_payload(tvbuff_t *tvb,
                                     int offset,
                                     packet_info *pinfo,
                                     proto_tree *top_level_tree _U_,
                                     proto_tree *spdy_tree,
                                     proto_item *spdy_proto,
                                     spdy_conv_t *conv_data,
                                     guint32 stream_id,
                                     const spdy_control_frame_info_t *frame)
{
  dissector_handle_t handle;
  guint num_data_frames;
  gboolean dissected;

  /
  proto_item_append_text(spdy_proto, ", Stream: %d, Length: %d",
                           stream_id,
                           frame->length);

  /
  proto_tree_add_item(spdy_tree, hf_spdy_data, tvb, offset, frame->length, ENC_NA);

  num_data_frames = spdy_get_num_data_frames(conv_data, stream_id);
  if (frame->length != 0 || num_data_frames != 0) {
    /
    tvbuff_t *next_tvb = NULL;
    tvbuff_t    *data_tvb = NULL;
    spdy_stream_info_t *si = NULL;
    guint8 *copied_data;
    gboolean is_single_chunk = FALSE;
    gboolean have_entire_body;
    char *media_str = NULL;

    /
    if (frame->length != 0) {
      next_tvb = tvb_new_subset_length(tvb, offset, frame->length);
      is_single_chunk = num_data_frames == 0 &&
          (frame->flags & SPDY_FLAG_FIN) != 0;
      if (!pinfo->fd->flags.visited) {
        if (!is_single_chunk) {
          if (spdy_assemble_entity_bodies) {
            copied_data = (guint8 *)tvb_memdup(wmem_file_scope(),next_tvb, 0, frame->length);
            spdy_add_data_chunk(conv_data, stream_id, pinfo->num, copied_data, frame->length);
          } else {
            spdy_increment_data_chunk_count(conv_data, stream_id);
          }
        }
      }
    } else {
      is_single_chunk = (num_data_frames == 1);
    }

    if (!(frame->flags & SPDY_FLAG_FIN)) {
      col_set_fence(pinfo->cinfo, COL_INFO);
      proto_item_append_text(spdy_proto, " (partial entity body)");
      /
      /
      goto body_dissected;
    }
    have_entire_body = is_single_chunk;
    /
    si = spdy_assemble_data_frames(conv_data, stream_id);
    if (si == NULL) {
      goto body_dissected;
    }
    data_tvb = si->assembled_data;
    if (spdy_assemble_entity_bodies) {
      have_entire_body = TRUE;
    }

    if (!have_entire_body) {
      goto body_dissected;
    }

    if (data_tvb == NULL) {
      if (next_tvb == NULL)
        goto body_dissected;
      data_tvb = next_tvb;
    } else {
      add_new_data_source(pinfo, data_tvb, "Assembled entity body");
    }

    if (have_entire_body && si->content_encoding != NULL &&
      g_ascii_strcasecmp(si->content_encoding, "identity") != 0) {
      /
      tvbuff_t *uncomp_tvb = NULL;
      proto_item *e_ti = NULL;
      proto_tree *e_tree = NULL;

      if (spdy_decompress_body &&
          (g_ascii_strcasecmp(si->content_encoding, "gzip") == 0 ||
           g_ascii_strcasecmp(si->content_encoding, "deflate") == 0)) {
        uncomp_tvb = tvb_child_uncompress(tvb, data_tvb, 0,
                                               tvb_reported_length(data_tvb));
      }
      /
      e_tree = proto_tree_add_subtree_format(spdy_tree, data_tvb,
                                 0, tvb_reported_length(data_tvb), ett_spdy_encoded_entity, &e_ti,
                                 "Content-encoded entity body (%s): %u bytes",
                                 si->content_encoding,
                                 tvb_reported_length(data_tvb));
      if (si->num_data_frames > 1) {
        wmem_list_t *dflist = si->data_frames;
        wmem_list_frame_t *frame_item;
        spdy_data_frame_t *df;
        guint32 framenum = 0;
        wmem_strbuf_t *str_frames = wmem_strbuf_new(wmem_packet_scope(), "");

        frame_item = wmem_list_frame_next(wmem_list_head(dflist));
        while (frame_item != NULL) {
          df = (spdy_data_frame_t *)wmem_list_frame_data(frame_item);
          if (framenum != df->framenum) {
            wmem_strbuf_append_printf(str_frames, " #%u", df->framenum);
            framenum = df->framenum;
          }
          frame_item = wmem_list_frame_next(frame_item);
        }

        proto_tree_add_expert_format(e_tree, pinfo, &ei_spdy_reassembly_info, data_tvb, 0,
                                    tvb_reported_length(data_tvb),
                                    "Assembled from %d frames in packet(s)%s",
                                    si->num_data_frames, wmem_strbuf_get_str(str_frames));
      }

      if (uncomp_tvb != NULL) {
        /

        /
        proto_item_append_text(e_ti, " -> %u bytes", tvb_reported_length(uncomp_tvb));
        data_tvb = uncomp_tvb;
        add_new_data_source(pinfo, data_tvb, "Uncompressed entity body");
      } else {
        if (spdy_decompress_body) {
          proto_item_append_text(e_ti, " [Error: Decompression failed]");
        }
        call_data_dissector(data_tvb, pinfo, e_tree);

        goto body_dissected;
      }
    }

    /

    if (have_entire_body && port_subdissector_table != NULL) {
      handle = dissector_get_uint_handle(port_subdissector_table,
                                         pinfo->match_uint);
    } else {
      handle = NULL;
    }
    if (handle == NULL && have_entire_body && si->content_type != NULL &&
      media_type_subdissector_table != NULL) {
      /
      if (si->content_type_parameters) {
        media_str = wmem_strdup(wmem_packet_scope(), si->content_type_parameters);
      }
      /
      pinfo->match_string = si->content_type;
      handle = dissector_get_string_handle(media_type_subdissector_table,
                                           si->content_type);
    }
    if (handle != NULL) {
      /
      dissected = call_dissector_with_data(handle, data_tvb, pinfo, spdy_tree, media_str);
    } else {
      dissected = FALSE;
    }

    if (!dissected && have_entire_body && si->content_type != NULL) {
      /
      call_dissector_with_data(media_handle, next_tvb, pinfo, spdy_tree, media_str);
    } else {
      /
      call_data_dissector(next_tvb, pinfo, spdy_tree);
    }

body_dissected:
    /
    ;
  }
  return frame->length;
}
