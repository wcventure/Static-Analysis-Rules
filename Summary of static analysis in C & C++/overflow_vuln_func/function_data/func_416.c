static void
dissect_evrc_aux(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, evrc_variant_t evrc_variant)
{
    guint8                      oct;
    guint8                      frame_count;
    guint8                      i;
    guint32                     offset, orig_offset, saved_offset;
    gboolean                    further_entries;
    guint32                     len;
    proto_item                  *item = NULL;
    proto_tree                  *evrc_tree = NULL;
    proto_tree                  *toc_tree = NULL;
    int                         hf_mode_request;
    int                         hf_toc_frame_type_high;
    int                         hf_toc_frame_type_low;

    /
    guint8                      speech_data_len[0x20];


    col_set_str(pinfo->cinfo, COL_PROTOCOL, "EVRC");

    if (!tree) return;

    offset = 0;
    orig_offset = offset;

    g_pinfo = pinfo;
    g_tree = tree;

    if (NULL == tree) return;

    len = tvb_length(tvb);

    item = proto_tree_add_item(tree, proto_evrc, tvb, 0, -1, ENC_NA);

    evrc_tree = proto_item_add_subtree(item, ett_evrc);

    proto_tree_add_item(evrc_tree, hf_evrc_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(evrc_tree, hf_evrc_interleave_length, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(evrc_tree, hf_evrc_interleave_index, tvb, offset, 1, ENC_BIG_ENDIAN);

    offset++;

    if (evrc_variant == EVRC_VARIANT_EVRC_LEGACY)
    {
        /

        frame_count = 0;
        further_entries = TRUE;
        while (further_entries &&
            ((len - (offset - orig_offset)) > 0))
        {
            item =
                proto_tree_add_text(evrc_tree, tvb, offset, 1, "ToC [%u]", frame_count+1);

            toc_tree = proto_item_add_subtree(item, ett_toc);

            proto_tree_add_item(toc_tree, hf_evrc_legacy_toc_fe_ind, tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(toc_tree, hf_evrc_legacy_toc_reduc_rate, tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(toc_tree, hf_evrc_legacy_toc_frame_type, tvb, offset, 1, ENC_BIG_ENDIAN);

            oct = tvb_get_guint8(tvb, offset);
            further_entries = (oct & 0x80) ? TRUE : FALSE;

            speech_data_len[frame_count] = evrc_frame_type_to_octs((guint8)(oct & 0x7f));

            frame_count++;
            offset++;
        }
    }
    else
    {
        /

        switch (evrc_variant)
        {
        case EVRC_VARIANT_EVRC:
            hf_mode_request = hf_evrc_mode_request;
            hf_toc_frame_type_high = hf_evrc_toc_frame_type_high;
            hf_toc_frame_type_low = hf_evrc_toc_frame_type_low;
            break;

        case EVRC_VARIANT_EVRC_B:
            hf_mode_request = hf_evrc_b_mode_request;
            hf_toc_frame_type_high = hf_evrc_b_toc_frame_type_high;
            hf_toc_frame_type_low = hf_evrc_b_toc_frame_type_low;
            break;

        case EVRC_VARIANT_EVRC_WB:
            hf_mode_request = hf_evrc_wb_mode_request;
            hf_toc_frame_type_high = hf_evrc_b_toc_frame_type_high;
            hf_toc_frame_type_low = hf_evrc_b_toc_frame_type_low;
            break;

        case EVRC_VARIANT_EVRC_NW:
            hf_mode_request = hf_evrc_nw_mode_request;
            hf_toc_frame_type_high = hf_evrc_b_toc_frame_type_high;
            hf_toc_frame_type_low = hf_evrc_b_toc_frame_type_low;
            break;

        default:
            return;
        }

        proto_tree_add_item(evrc_tree, hf_mode_request, tvb, offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(evrc_tree, hf_evrc_frame_count, tvb, offset, 1, ENC_BIG_ENDIAN);

        /
        frame_count = (tvb_get_guint8(tvb, offset) & 0x1f) + 1;

        offset++;
        saved_offset = offset;

        item =
            proto_tree_add_text(evrc_tree, tvb, offset, -1, "ToC - %u frame%s",
                frame_count, plurality(frame_count, "", "s"));

        toc_tree = proto_item_add_subtree(item, ett_toc);

        i = 0;
        while ((i < frame_count) &&
            ((len - (offset - orig_offset)) > 0))
        {
            oct = tvb_get_guint8(tvb, offset);

            proto_tree_add_item(toc_tree, hf_toc_frame_type_high, tvb, offset, 1, ENC_BIG_ENDIAN);

            speech_data_len[i] = evrc_frame_type_to_octs((guint8)((oct & 0xf0) >> 4));

            i++;

	    if (i < frame_count)
            {
                /
                proto_tree_add_item(toc_tree, hf_toc_frame_type_low, tvb, offset, 1, ENC_BIG_ENDIAN);

                speech_data_len[i] = evrc_frame_type_to_octs((guint8)(oct & 0x0f));

                i++;
            }

            offset++;
        }

        if (frame_count & 0x01)
        {
            /
            proto_tree_add_item(toc_tree, hf_evrc_padding, tvb, offset-1, 1, ENC_BIG_ENDIAN);
        }

        proto_item_set_len(item, offset - saved_offset);
    }

    i = 0;
    while ((i < frame_count) &&
        ((len - (offset - orig_offset)) >= speech_data_len[i]))
    {
        proto_tree_add_text(evrc_tree, tvb, offset, speech_data_len[i], "Speech Data [%u]", i+1);

        offset += speech_data_len[i];
        i++;
    }
}
