static int
dissect_ieee802154_6top(tvbuff_t *tvb, packet_info *pinfo, proto_tree *p_inf_elem_tree, guint offset, gint pie_length)
{
    const guint8 supported_6p_version = 0x00;

    guint8     subie;
    guint8     version;
    guint8     type;
    guint8     code;
    guint8     num_cells;
    int        orig_offset = offset;
    gboolean   have_cell_list = FALSE;
    int        i;
    proto_item *sixtop_item = NULL;
    proto_tree *sixtop_tree = NULL;
    proto_item *cell_list_item = NULL;
    proto_tree *cell_list_tree = NULL;
    proto_item *cell_item = NULL;
    proto_tree *cell_tree = NULL;
    proto_item *type_item = NULL;
    proto_item *code_item = NULL;
    const gchar *code_str = NULL;
    static const int * cell_options[] = {
        &hf_ieee802154_6top_cell_option_tx,
        &hf_ieee802154_6top_cell_option_rx,
        &hf_ieee802154_6top_cell_option_shared,
        &hf_ieee802154_6top_cell_option_reserved,
        NULL
    };

    if (pie_length < 5) {
        return pie_length;
    }

    subie = tvb_get_guint8(tvb, offset);
    version =  tvb_get_guint8(tvb, offset + 1) & IETF_6TOP_VERSION;

    if (subie != IEEE802154_IETF_SUBIE_6TOP || version != supported_6p_version) {
        return pie_length;
    }

    type = (tvb_get_guint8(tvb, offset + 1) & IETF_6TOP_TYPE) >> 4;
    code = tvb_get_guint8(tvb, offset + 2);

    proto_tree_add_item(p_inf_elem_tree, hf_ieee802154_p_ie_ietf_sub_id, tvb, offset, 1, ENC_LITTLE_ENDIAN);

    sixtop_item = proto_tree_add_item(p_inf_elem_tree, hf_ieee802154_6top, tvb, offset, pie_length, ENC_NA);
    sixtop_tree = proto_item_add_subtree(sixtop_item, ett_ieee802154_p_ie_6top);

    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_version, tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
    type_item = proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_type, tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_flags_reserved, tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
    code_item = proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_code, tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_sfid, tvb, offset + 3, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_seqnum, tvb, offset + 4, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_gen, tvb, offset + 4, 1, ENC_LITTLE_ENDIAN);

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "6top");
    if (type == IETF_6TOP_TYPE_REQUEST) {
      code_str = val_to_str_const(code, ietf_6top_command_identifiers,"Unknown");
      col_add_fstr(pinfo->cinfo, COL_INFO, "6P %s Request", code_str);
    } else {
      code_str = val_to_str_const(code, ietf_6top_return_codes,"Unknown");
      col_add_fstr(pinfo->cinfo, COL_INFO, "6P %s (%s)",
                   val_to_str_const(type, ietf_6top_types,"Unknown"), code_str);
    }
    proto_item_append_text(code_item, " (%s)", code_str);

    offset += 5;
    pie_length -= 5;

    if (type == IETF_6TOP_TYPE_REQUEST) {
        switch (code) {
        case IETF_6TOP_CMD_ADD:
        case IETF_6TOP_CMD_DELETE:
        case IETF_6TOP_CMD_RELOCATE:
            if (pie_length < 4) {
                break;
            }
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_metadata, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_tree_add_bitmask(sixtop_tree, tvb, offset + 2, hf_ieee802154_6top_cell_options, ett_ieee802154_p_ie_6top_cell_options, cell_options, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_num_cells, tvb, offset + 3, 1, ENC_LITTLE_ENDIAN);
            num_cells = tvb_get_guint8(tvb, offset + 3);
            pie_length -= 4;
            offset += 4;
            if (pie_length > 0 && (pie_length % 4) == 0) {
                have_cell_list = TRUE;
            }
            break;
        case IETF_6TOP_CMD_COUNT:
            if (pie_length < 3) {
                break;
            }
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_metadata, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_tree_add_bitmask(sixtop_tree, tvb, offset + 2, hf_ieee802154_6top_cell_options, ett_ieee802154_p_ie_6top_cell_options, cell_options, ENC_LITTLE_ENDIAN);
            pie_length -= 3;
            offset += 3;
            break;
        case IETF_6TOP_CMD_LIST:
            if (pie_length != 8) {
                break;
            }
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_metadata, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            proto_tree_add_bitmask(sixtop_tree, tvb, offset + 2, hf_ieee802154_6top_cell_options, ett_ieee802154_p_ie_6top_cell_options, cell_options, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_reserved, tvb, offset + 3, 1, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_offset, tvb, offset + 4, 2, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_max_num_cells, tvb, offset + 6, 2, ENC_LITTLE_ENDIAN);
            pie_length -= 8;
            offset += 8;
            break;
        case IETF_6TOP_CMD_CLEAR:
            if (pie_length < 2) {
                break;
            }
            proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_metadata, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            pie_length -= 2;
            offset += 2;
            break;
        default:
            /
            expert_add_info(pinfo, code_item, &ei_ieee802154_6top_unsupported_command);
            break;
        }
    } else if (type == IETF_6TOP_TYPE_RESPONSE || type == IETF_6TOP_TYPE_CONFIRMATION) {
        switch(code) {
        case IETF_6TOP_RC_SUCCESS:
            if (pie_length > 0) {
                if (pie_length == 2) {
                    proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_total_num_cells, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                    pie_length -= 2;
                    offset += 2;
                } else if (pie_length > 0 && (pie_length % 4) == 0) {
                    have_cell_list = TRUE;
                }
            }
            break;
        case IETF_6TOP_RC_EOL:
            if(pie_length > 0 && (pie_length % 4) == 0) {
                have_cell_list = TRUE;
            }
            break;
        case IETF_6TOP_RC_ERROR:
        case IETF_6TOP_RC_RESET:
        case IETF_6TOP_RC_VER_ERR:
        case IETF_6TOP_RC_SFID_ERR:
        case IETF_6TOP_RC_GEN_ERR:
        case IETF_6TOP_RC_BUSY:
        case IETF_6TOP_RC_NORES:
        case IETF_6TOP_RC_CELLLIST_ERR:
            /
            break;
        default:
            /
            expert_add_info(pinfo, code_item, &ei_ieee802154_6top_unsupported_return_code);
            break;
        }
    } else {
        /
        expert_add_info(pinfo, type_item, &ei_ieee802154_6top_unsupported_type);
    }

    if (have_cell_list) {
        if (type == IETF_6TOP_TYPE_REQUEST && code == IETF_6TOP_CMD_RELOCATE) {
            cell_list_item = proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_rel_cell_list, tvb, offset, pie_length, ENC_NA);
            cell_list_tree = proto_item_add_subtree(cell_list_item, ett_ieee802154_p_ie_6top_rel_cell_list);
            /
            for (i = 0; i < num_cells; offset += 4, i++) {
                cell_item = proto_tree_add_item(cell_list_tree, hf_ieee802154_6top_cell, tvb, offset, 4, ENC_NA);
                cell_tree = proto_item_add_subtree(cell_item, ett_ieee802154_p_ie_6top_cell);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_slot_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_channel_offset, tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
            }
            pie_length -= num_cells * 4;
            cell_list_item = proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_cand_cell_list, tvb, offset, pie_length, ENC_NA);
            cell_list_tree = proto_item_add_subtree(cell_list_item, ett_ieee802154_p_ie_6top_cand_cell_list);
            for (i = 0; pie_length > 0; pie_length -= 4, offset += 4, i++) {
                cell_item = proto_tree_add_item(cell_list_tree, hf_ieee802154_6top_cell, tvb, offset, 4, ENC_NA);
                cell_tree = proto_item_add_subtree(cell_item, ett_ieee802154_p_ie_6top_cell);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_slot_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_channel_offset, tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
            }
        } else {
            cell_list_item = proto_tree_add_item(sixtop_tree, hf_ieee802154_6top_cell_list, tvb, offset, pie_length, ENC_NA);
            cell_list_tree = proto_item_add_subtree(cell_list_item, ett_ieee802154_p_ie_6top_cell_list);
            for (i = 0; pie_length > 0; pie_length -= 4, offset += 4, i++) {
                cell_item = proto_tree_add_item(cell_list_tree, hf_ieee802154_6top_cell, tvb, offset, 4, ENC_NA);
                cell_tree = proto_item_add_subtree(cell_item, ett_ieee802154_p_ie_6top_cell);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_slot_offset, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(cell_tree, hf_ieee802154_6top_channel_offset, tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
            }
        }
    }

    return offset - orig_offset;
} /
