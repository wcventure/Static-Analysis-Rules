void
dissect_nmas_request(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ncp_tree, ncp_req_hash_value *request_value)
{
    guint8              func, subfunc = 0;
    guint32             msg_length=0, cur_string_len=0;
    guint32             foffset;
    guint32             subverb=0;
    guint32             attribute=0;
    guint8              msgverb=0;
    proto_tree          *atree;
    proto_item          *aitem;
    
    foffset = 6;
    func = tvb_get_guint8(tvb, foffset);
    foffset += 1;
    subfunc = tvb_get_guint8(tvb, foffset);
    foffset += 1;
    
    /
    if (check_col(pinfo->cinfo, COL_INFO)) {
       col_set_str(pinfo->cinfo, COL_PROTOCOL, "NMAS");
       col_add_fstr(pinfo->cinfo, COL_INFO, "C NMAS - %s",
                    val_to_str(subfunc, nmas_func_enum, "Unknown (0x%02x)"));
    }
    aitem = proto_tree_add_text(ncp_tree, tvb, foffset, -1, "Packet Type: %s",
                                val_to_str(subfunc, nmas_func_enum, "Unknown (0x%02x)"));
    atree = proto_item_add_subtree(aitem, ett_nmas);
    switch (subfunc) {
    case 1:
        proto_tree_add_item(atree, hf_ping_version, tvb, foffset, 4, TRUE);
        foffset += 4;
        proto_tree_add_item(atree, hf_ping_flags, tvb, foffset, 4, TRUE);
        foffset += 4;
        break;
    case 2:
        proto_tree_add_item(atree, hf_frag_handle, tvb, foffset, 4, TRUE);
        /
        if (tvb_get_letohl(tvb, foffset)!=0xffffffff) {
            break;
        }
        foffset += 4;
        foffset += 4; /
        proto_tree_add_item(atree, hf_length, tvb, foffset, 4, TRUE);
        msg_length = tvb_get_letohl(tvb, foffset);
        foffset += 4;
        foffset += 12;
        msg_length -= 16;
        proto_tree_add_item(atree, hf_subverb, tvb, foffset, 4, TRUE);
        subverb = tvb_get_letohl(tvb, foffset);
        if (request_value) {
            request_value->req_nds_flags=subverb; /
        }
        foffset += 4;
        msg_length -= 4;
        if (check_col(pinfo->cinfo, COL_INFO)) {
            col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
                            val_to_str(subverb, nmas_subverb_enum, "Unknown subverb (%u)"));
        }
        switch (subverb) {
        case 0:             /
            proto_tree_add_item(atree, hf_ping_version, tvb, foffset, 4, TRUE);
            foffset += 4;
            proto_tree_add_item(atree, hf_ping_flags, tvb, foffset, 4, TRUE);
            foffset += 4;
            break;
        case 2:             /
            proto_tree_add_item(atree, hf_opaque, tvb, foffset, msg_length, FALSE);
            foffset += msg_length;
            break;
        case 4:             /
        case 6:             /
            /
            break;
        case 8:             /
            proto_tree_add_item(atree, hf_reply_buffer_size, tvb, foffset, 1, TRUE);
            foffset += 4;
            msgverb = tvb_get_guint8(tvb, foffset); 
            if (request_value) {
                request_value->nds_request_verb=msgverb; /
            }
            proto_tree_add_item(atree, hf_lsm_verb, tvb, foffset, 1, TRUE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO)) {
                col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
                                val_to_str(msgverb, nmas_lsmverb_enum, "Unknown (%u)"));
            }
            switch (msgverb)
            {
            case 1:
                break;
            case 2:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            default:
                break;
            }
            break;
        case 10:            /
            /
            foffset += 4; 
            /
            foffset += 8;
            foffset = nmas_string(tvb, hf_tree, atree, foffset, TRUE);
            foffset = nmas_string(tvb, hf_user, atree, foffset, TRUE);
            break;
        case 1242:          /
            foffset += 4;
            proto_tree_add_item(atree, hf_msg_version, tvb, foffset, 4, FALSE);
            foffset += 4;
            proto_tree_add_item(atree, hf_session_ident, tvb, foffset, 4, FALSE);
            foffset += 4;
            foffset += 3;
            msgverb = tvb_get_guint8(tvb, foffset);
            if (request_value) {
                request_value->nds_request_verb=msgverb; /
            }
            proto_tree_add_item(atree, hf_msg_verb, tvb, foffset, 1, FALSE);
            foffset += 1;
            msg_length -= 12;
            if (check_col(pinfo->cinfo, COL_INFO)) {
                col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
                                val_to_str(msgverb, nmas_msgverb_enum, "Unknown (%u)"));
            }
            switch(msgverb)
            {
            case 1:
                msg_length = tvb_get_ntohl(tvb, foffset);
                proto_tree_add_item(atree, hf_length, tvb, foffset, 4, FALSE);
                foffset += 4;
                proto_tree_add_item(atree, hf_data, tvb, foffset, msg_length, FALSE);
                foffset += msg_length;
                break;
            case 3:
                msg_length = tvb_get_ntohl(tvb, foffset);
                msg_length -= 4;
                proto_tree_add_item(atree, hf_length, tvb, foffset, 4, FALSE);
                foffset += 4;
                while (msg_length > 0)
                {
                    attribute = tvb_get_ntohl(tvb, foffset);
                    foffset += 4;
                    cur_string_len=tvb_get_ntohl(tvb, foffset);
                    switch (attribute) {
                    case 1:
                        foffset = nmas_string(tvb, hf_user, atree, foffset, FALSE);
                        break;
                    case 2:
                        foffset = nmas_string(tvb, hf_tree, atree, foffset, FALSE);
                        break;
                    case 4:
                        foffset = nmas_string(tvb, hf_clearence, atree, foffset, FALSE);
                        break;
                    case 11:
                        foffset = nmas_string(tvb, hf_login_sequence, atree, foffset, FALSE);
                        break;
                    default:
                        break;
                    }
                    msg_length -= cur_string_len;
                    if (tvb_reported_length_remaining(tvb, foffset)<5)
                    {
                        break;
                    }
                }
                break;
            case 5:
                proto_tree_add_item(atree, hf_opaque, tvb, foffset, tvb_reported_length_remaining(tvb, foffset), FALSE);
                foffset += msg_length;
                break;
            case 7:
            case 9:
                /
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case 3:
        /
        break;
    default:
        break;
    }
}
