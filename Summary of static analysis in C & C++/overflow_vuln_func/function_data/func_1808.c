void
dissect_nmas_reply(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ncp_tree, guint8 func _U_, guint8 subfunc, ncp_req_hash_value	*request_value)
{
    guint32             foffset=0, roffset=0;
    guint32             subverb=0;
    guint8              msgverb=0;
    guint32             msg_length=0;
    guint32             return_code=0, encrypt_error=0;
    proto_tree          *atree;
    proto_item          *aitem;
    
    foffset = 8;
    if (request_value) {
        subverb = request_value->req_nds_flags;
        msgverb = request_value->nds_request_verb;
    }
    if (check_col(pinfo->cinfo, COL_INFO)) {
       col_set_str(pinfo->cinfo, COL_PROTOCOL, "NMAS");
    }
    if (tvb_reported_length_remaining(tvb, foffset)<4) {
        return;
    }
        
    aitem = proto_tree_add_text(ncp_tree, tvb, foffset, -1, "Packet Type: %s",
                                val_to_str(subfunc, nmas_func_enum, "Unknown (0x%02x)"));
    atree = proto_item_add_subtree(aitem, ett_nmas);
    switch (subfunc) {
    case 1:
        proto_tree_add_item(atree, hf_ping_flags, tvb, foffset, 4, TRUE);
        foffset += 4;
        proto_tree_add_item(atree, hf_nmas_version, tvb, foffset, 4, TRUE);
        foffset += 4;
        break;
    case 2:
        proto_tree_add_text(atree, tvb, foffset, -1, "Verb: %s",
                            val_to_str(subverb, nmas_subverb_enum, "Unknown (%u)"));
        proto_tree_add_item(atree, hf_length, tvb, foffset, 4, TRUE);
        msg_length = tvb_get_letohl(tvb, foffset);
        foffset +=4;
        proto_tree_add_item(atree, hf_frag_handle, tvb, foffset, 4, TRUE);
        /
        if (tvb_get_letohl(tvb, foffset)!=0xffffffff) {
            break;
        }
        foffset += 4;
        return_code = tvb_get_letohl(tvb, foffset);
        roffset = foffset;
        foffset += 4;
        msg_length -= 8;
        if (return_code == 0 && msg_length > 0)
        {
            switch (subverb) {
            case 0:             /
                proto_tree_add_item(atree, hf_ping_flags, tvb, foffset, 4, TRUE);
                foffset += 4;
                proto_tree_add_item(atree, hf_nmas_version, tvb, foffset, 4, TRUE);
                foffset += 4;
                break;
            case 2:             /
                proto_tree_add_item(atree, hf_squeue_bytes, tvb, foffset, 4, TRUE);
                foffset += 4;
                proto_tree_add_item(atree, hf_cqueue_bytes, tvb, foffset, 4, TRUE);
                foffset += 4;
                break;
            case 4:             /
                proto_tree_add_item(atree, hf_opaque, tvb, foffset, msg_length, TRUE);
                foffset += msg_length;
                break;
            case 6:             /
                proto_tree_add_item(atree, hf_num_creds, tvb, foffset, 4, TRUE);
                foffset += 4;
                proto_tree_add_item(atree, hf_cred_type, tvb, foffset, 4, TRUE);
                foffset += 4;
                proto_tree_add_item(atree, hf_login_state, tvb, foffset, 4, TRUE);
                foffset += 4;
                msg_length -= 12;
                proto_tree_add_item(atree, hf_enc_cred, tvb, foffset, msg_length, TRUE);
                foffset += msg_length;
                break;
            case 8:             /
                proto_tree_add_text(atree, tvb, foffset, -1, "Subverb: %s",
                                    val_to_str(msgverb, nmas_lsmverb_enum, "Unknown (%u)"));
                switch(msgverb)
                {
                    /
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                    proto_tree_add_item(atree, hf_enc_data, tvb, foffset, msg_length, TRUE);
                    foffset += msg_length;
                    break;
                default:
                    break;
                }
                break;
            case 10:            /
                proto_tree_add_item(atree, hf_nmas_version, tvb, foffset, 4, TRUE);
                foffset += 4;
                break;
            case 1242:          /
                proto_tree_add_text(atree, tvb, foffset, -1, "Subverb: %s",
                                    val_to_str(msgverb, nmas_msgverb_enum, "Unknown (%u)"));
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
                    proto_tree_add_item(atree, hf_session_ident, tvb, foffset, 4, FALSE);
                    foffset += 4;
                    break;
                case 5:
                    /
                    break;
                case 7:
                    encrypt_error = tvb_get_ntohl(tvb, foffset);
                    if (match_strval(encrypt_error, nmas_errors_enum)!=NULL)
                    {
                        if (check_col(pinfo->cinfo, COL_INFO)) {
                           col_add_fstr(pinfo->cinfo, COL_INFO, "R Payload Error - %s", match_strval(encrypt_error, nmas_errors_enum));
                        }
                        expert_item = proto_tree_add_item(atree, hf_encrypt_error, tvb, foffset, 4, FALSE);
                        expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "NMAS Payload Error: %s", match_strval(encrypt_error, nmas_errors_enum));
                    }
                    else
                    {
                        proto_tree_add_item(atree, hf_opaque, tvb, foffset, msg_length, FALSE);
                    }
                    foffset += msg_length;
                    break;
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
        }
        if (match_strval(return_code, nmas_errors_enum)!=NULL) 
        {
            expert_item = proto_tree_add_item(atree, hf_return_code, tvb, roffset, 4, TRUE);
            expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "NMAS Error: 0x%08x %s", return_code, match_strval(return_code, nmas_errors_enum));
            if (check_col(pinfo->cinfo, COL_INFO)) {
               col_add_fstr(pinfo->cinfo, COL_INFO, "R Error - %s", match_strval(return_code, nmas_errors_enum));
            }
        }
        else
        {
            if (return_code!=0)
            {
                expert_item = proto_tree_add_item(atree, hf_return_code, tvb, roffset, 4, TRUE);
                expert_add_info_format(pinfo, expert_item, PI_RESPONSE_CODE, PI_ERROR, "NMAS Error: 0x%08x is unknown", return_code);
                if (check_col(pinfo->cinfo, COL_INFO)) {
                   col_add_fstr(pinfo->cinfo, COL_INFO, "R Unknown NMAS Error - 0x%08x", return_code);
                }
            }
        }

        if (return_code == 0) {
            proto_tree_add_text(atree, tvb, roffset, 4, "Return Code: Success (0x00000000)");
        }
        break;
    case 3:
        break;
    default:
        break;
    }
}
