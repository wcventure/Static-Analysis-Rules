static int
dissect_ipp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree  *ipp_tree;
    proto_item  *ti;
    int          offset     = 0;
    gboolean     is_request = (pinfo->destport == pinfo->match_uint);
    /
    guint16      status_code;
    const gchar *status_type;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "IPP");
    if (is_request)
        col_set_str(pinfo->cinfo, COL_INFO, "IPP request");
    else
        col_set_str(pinfo->cinfo, COL_INFO, "IPP response");

    ti = proto_tree_add_item(tree, proto_ipp, tvb, offset, -1, ENC_NA);
    ipp_tree = proto_item_add_subtree(ti, ett_ipp);

    proto_tree_add_item(ipp_tree, hf_ipp_version, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    if (is_request) {
        proto_tree_add_item(ipp_tree, hf_ipp_operation_id, tvb, offset, 2, ENC_BIG_ENDIAN);
    } else {
        status_code = tvb_get_ntohs(tvb, offset);
        switch (status_code & STATUS_TYPE_MASK) {

        case STATUS_SUCCESSFUL:
            status_type = "Successful";
            break;

        case STATUS_INFORMATIONAL:
            status_type = "Informational";
            break;

        case STATUS_REDIRECTION:
            status_type = "Redirection";
            break;

        case STATUS_CLIENT_ERROR:
            status_type = "Client error";
            break;

        case STATUS_SERVER_ERROR:
            status_type = "Server error";
            break;

        default:
            status_type = "Unknown";
            break;
        }
        proto_tree_add_uint_format_value(ipp_tree, hf_ipp_status_code, tvb, offset, 2, status_code,
                            "%s (%s)", status_type, val_to_str(status_code, status_vals, "0x804x"));
    }
    offset += 2;

    proto_tree_add_item(ipp_tree, hf_ipp_request_id, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset += 4;

    offset = parse_attributes(tvb, offset, ipp_tree);

    if (tvb_offset_exists(tvb, offset)) {
        call_data_dissector(tvb_new_subset_remaining(tvb, offset), pinfo,
                        ipp_tree);
    }
    return tvb_captured_length(tvb);
}
