static int dissect_spdy_header_payload(
  tvbuff_t *tvb,
  int offset,
  packet_info *pinfo,
  proto_tree *frame_tree,
  const spdy_control_frame_info_t *frame,
  spdy_conv_t *conv_data) {
  guint32 stream_id;
  int header_block_length = frame->length;
  int hdr_offset = 0;
  tvbuff_t *header_tvb = NULL;
  const gchar *hdr_method = NULL;
  const gchar *hdr_path = NULL;
  const gchar *hdr_version = NULL;
  const gchar *hdr_host = NULL;
  const gchar *hdr_scheme = NULL;
  const gchar *hdr_status = NULL;
  gchar *content_type = NULL;
  gchar *content_encoding = NULL;
  guint32 num_headers = 0;
  proto_item *header_block_item;
  proto_tree *header_block_tree;

  /
  stream_id = tvb_get_ntohl(tvb, offset) & SPDY_STREAM_ID_MASK;
  dissect_spdy_stream_id_field(tvb, offset, pinfo, frame_tree, hf_spdy_streamid);
  offset += 4;

  /
  if (frame->type == SPDY_SYN_STREAM) {
    /
    dissect_spdy_stream_id_field(tvb, offset, pinfo, frame_tree, hf_spdy_associated_streamid);
    offset += 4;

    /
    proto_tree_add_item(frame_tree, hf_spdy_priority, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item(frame_tree, hf_spdy_unused, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item(frame_tree, hf_spdy_slot, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;
  }


  /
  switch (frame->type) {
    case SPDY_SYN_STREAM:
      header_block_length -= 10;
      break;
    case SPDY_SYN_REPLY:
    case SPDY_HEADERS:
      header_block_length -= 4;
      break;
    default:
      /
      DISSECTOR_ASSERT_NOT_REACHED();
  }

  /
  header_block_item = proto_tree_add_item(frame_tree,
                                            hf_spdy_header_block,
                                            tvb,
                                            offset,
                                            header_block_length,
                                            ENC_NA);
  header_block_tree = proto_item_add_subtree(header_block_item,
                                               ett_spdy_header_block);

  /
  if (!spdy_decompress_headers) {
    header_tvb = tvb;
    hdr_offset = offset;
  } else {
    spdy_header_info_t *header_info;

    /
    header_info = spdy_find_saved_header_block(pinfo,
                                               stream_id,
                                               frame->type);

    /
    if (header_info == NULL) {
      guint8 *uncomp_ptr = NULL;
      guint uncomp_length = 0;
#ifdef HAVE_ZLIB
      z_streamp decomp;

      /
      if (stream_id % 2 == 0) {
        /
        decomp = conv_data->rply_decompressor;
      } else if (frame->type == SPDY_HEADERS) {
        /
        decomp = conv_data->rply_decompressor;
      } else if (frame->type == SPDY_SYN_STREAM) {
        decomp = conv_data->rqst_decompressor;
      } else if (frame->type == SPDY_SYN_REPLY) {
        decomp = conv_data->rply_decompressor;
      } else {
        /
        DISSECTOR_ASSERT_NOT_REACHED();
      }

      /
      uncomp_ptr = spdy_decompress_header_block(tvb,
                                                decomp,
                                                conv_data->dictionary_id,
                                                offset,
                                                header_block_length,
                                                &uncomp_length);

      /
      if (uncomp_ptr == NULL) {
        expert_add_info(pinfo, frame_tree, &ei_spdy_inflation_failed);

        proto_item_append_text(frame_tree, " [Error: Header decompression failed]");
        return -1;
      }
#endif

      /
      header_info = spdy_save_header_block(pinfo, stream_id, frame->type, uncomp_ptr, uncomp_length);
    }

    /
    header_tvb = tvb_new_child_real_data(tvb, header_info->header_block,
                                         header_info->header_block_len,
                                         header_info->header_block_len);
    add_new_data_source(pinfo, header_tvb, "Uncompressed headers");
    hdr_offset = 0;
  }

  /
  if (header_tvb == NULL || !spdy_decompress_headers) {
    num_headers = 0;
  } else {
    num_headers = tvb_get_ntohl(header_tvb, hdr_offset);
    / proto_tree_add_item(header_block_tree,
                             hf_spdy_num_headers,
                             header_tvb,
                             hdr_offset,
                             4,
                             ENC_BIG_ENDIAN);
  }
  hdr_offset += 4;

  /
  while (num_headers--) {
    gchar *header_name;
    const gchar *header_value;
    proto_tree *header_tree;
    proto_item *header;
    int header_name_offset;
    int header_value_offset;
    int header_name_length;
    int header_value_length;

    /
    if (tvb_reported_length_remaining(header_tvb, hdr_offset) < 4) {
      expert_add_info_format(pinfo, frame_tree, &ei_spdy_mal_frame_data,
                             "Not enough frame data for header name size.");
      break;
    }
    header_name_offset = hdr_offset;
    header_name_length = tvb_get_ntohl(header_tvb, hdr_offset);
    hdr_offset += 4;
    if (tvb_reported_length_remaining(header_tvb, hdr_offset) < header_name_length) {
      expert_add_info_format(pinfo, frame_tree, &ei_spdy_mal_frame_data,
                             "Not enough frame data for header name.");
      break;
    }
    header_name = (gchar *)tvb_get_string_enc(wmem_packet_scope(), header_tvb,
                                                    hdr_offset,
                                                    header_name_length, ENC_ASCII|ENC_NA);
    hdr_offset += header_name_length;

    /
    if (tvb_reported_length_remaining(header_tvb, hdr_offset) < 4) {
      expert_add_info_format(pinfo, frame_tree, &ei_spdy_mal_frame_data,
                             "Not enough frame data for header value size.");
      break;
    }
    header_value_offset = hdr_offset;
    header_value_length = tvb_get_ntohl(header_tvb, hdr_offset);
    hdr_offset += 4;
    if (tvb_reported_length_remaining(header_tvb, hdr_offset) < header_value_length) {
      expert_add_info_format(pinfo, frame_tree, &ei_spdy_mal_frame_data,
                             "Not enough frame data for header value.");
      break;
    }
    header_value = (gchar *)tvb_get_string_enc(wmem_packet_scope(),header_tvb,
                                                     hdr_offset,
                                                     header_value_length, ENC_ASCII|ENC_NA);
    hdr_offset += header_value_length;

    /
    if (frame_tree) {
      /
      header = proto_tree_add_item(frame_tree,
                                   hf_spdy_header,
                                   header_tvb,
                                   header_name_offset,
                                   hdr_offset - header_name_offset,
                                   ENC_NA);
      proto_item_append_text(header, ": %s: %s", header_name, header_value);
      header_tree = proto_item_add_subtree(header, ett_spdy_header);

      /
      proto_tree_add_item(header_tree, hf_spdy_header_name, header_tvb,
                          header_name_offset, 4, ENC_ASCII|ENC_BIG_ENDIAN);

      /
      proto_tree_add_item(header_tree, hf_spdy_header_value, header_tvb,
                          header_value_offset, 4, ENC_ASCII|ENC_BIG_ENDIAN);
    }

    /
    /
    if (g_strcmp0(header_name, ":method") == 0) {
      hdr_method = header_value;
    } else if (g_strcmp0(header_name, ":path") == 0) {
      hdr_path = header_value;
    } else if (g_strcmp0(header_name, ":version") == 0) {
      hdr_version = header_value;
    } else if (g_strcmp0(header_name, ":host") == 0) {
      hdr_host = header_value;
    } else if (g_strcmp0(header_name, ":scheme") == 0) {
      hdr_scheme = header_value;
    } else if (g_strcmp0(header_name, ":status") == 0) {
      hdr_status = header_value;
    } else if (g_strcmp0(header_name, "content-type") == 0) {
      content_type = wmem_strdup(wmem_file_scope(), header_value);
    } else if (g_strcmp0(header_name, "content-encoding") == 0) {
      content_encoding = wmem_strdup(wmem_file_scope(), header_value);
    }
  }

  /
  if (hdr_version != NULL) {
    if (hdr_status == NULL) {
      proto_item_append_text(frame_tree, ", Request: %s %s://%s%s %s",
                      hdr_method, hdr_scheme, hdr_host, hdr_path, hdr_version);
    } else {
      proto_item_append_text(frame_tree, ", Response: %s %s",
                      hdr_status, hdr_version);
    }
  }

  /
  if (content_type != NULL && !pinfo->fd->flags.visited) {
    gchar *content_type_params = spdy_parse_content_type(content_type);
    spdy_save_stream_info(conv_data, stream_id, content_type,
                          content_type_params, content_encoding);
  }

  return frame->length;
}
