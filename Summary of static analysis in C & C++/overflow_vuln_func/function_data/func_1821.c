static void
dissect_ndps(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ndps_tree)
{
    guint32     ndps_xid;
    guint32     ndps_prog;
    guint32     ndps_packet_type;
    guint32     ndps_rpc_version;
    int         foffset;
    guint32     ndps_hfname;
    guint32     ndps_func;
    const char  *ndps_program_string;
    const char  *ndps_func_string;


    ndps_packet_type = tvb_get_ntohl(tvb, 8);
    if (ndps_packet_type != 0 && ndps_packet_type != 1) {     /
        if (check_col(pinfo->cinfo, COL_INFO))
            col_set_str(pinfo->cinfo, COL_INFO, "(Continuation Data)");
        proto_tree_add_text(ndps_tree, tvb, 0, tvb_length_remaining(tvb, 0), "Data - (%d Bytes)", tvb_length_remaining(tvb, 0));
        return;
    }
    foffset = 0;
    proto_tree_add_item(ndps_tree, hf_ndps_record_mark, tvb,
                   foffset, 2, FALSE);
    foffset += 2;
    proto_tree_add_item(ndps_tree, hf_ndps_length, tvb,
                   foffset, 2, FALSE);
    foffset += 2;

    ndps_xid = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_uint(ndps_tree, hf_ndps_xid, tvb, foffset, 4, ndps_xid);
    foffset += 4;
    ndps_packet_type = tvb_get_ntohl(tvb, foffset);
    proto_tree_add_uint(ndps_tree, hf_ndps_packet_type, tvb, foffset, 4, ndps_packet_type);
    foffset += 4;
    if(ndps_packet_type == 0x00000001)          /
    {
        if (check_col(pinfo->cinfo, COL_INFO))
            col_set_str(pinfo->cinfo, COL_INFO, "R NDPS ");
        proto_tree_add_item(ndps_tree, hf_ndps_rpc_accept, tvb, foffset, 4, FALSE);
        if (tvb_get_ntohl(tvb, foffset)==0) {
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_auth_null, tvb, foffset, 8, FALSE);
            foffset += 8;
        }
        else
        {
            foffset += 4;
            proto_tree_add_item(ndps_tree, hf_ndps_rpc_rej_stat, tvb, foffset+4, 4, FALSE);
            foffset += 4;
        }
        dissect_ndps_reply(tvb, pinfo, ndps_tree, foffset);
    }
    else
    {
        if (check_col(pinfo->cinfo, COL_INFO))
            col_set_str(pinfo->cinfo, COL_INFO, "C NDPS ");
        ndps_rpc_version = tvb_get_ntohl(tvb, foffset);
        proto_tree_add_item(ndps_tree, hf_ndps_rpc_version, tvb, foffset, 4, FALSE);
        foffset += 4;
        ndps_prog = tvb_get_ntohl(tvb, foffset);
        ndps_program_string = match_strval(ndps_prog, spx_ndps_program_vals);
        if( ndps_program_string != NULL)
        {
            proto_tree_add_item(ndps_tree, hf_spx_ndps_program, tvb, foffset, 4, FALSE);
            foffset += 4;
            if (check_col(pinfo->cinfo, COL_INFO))
            {
                col_append_str(pinfo->cinfo, COL_INFO, (const gchar*) ndps_program_string);
                col_append_str(pinfo->cinfo, COL_INFO, ", ");
            }
            proto_tree_add_item(ndps_tree, hf_spx_ndps_version, tvb, foffset, 4, FALSE);
            foffset += 4;
            ndps_func = tvb_get_ntohl(tvb, foffset);
            switch(ndps_prog)
            {
                case 0x060976:
                    ndps_hfname = hf_spx_ndps_func_print;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_print_func_vals);
                    break;
                case 0x060977:
                    ndps_hfname = hf_spx_ndps_func_broker;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_broker_func_vals);
                    break;
                case 0x060978:
                    ndps_hfname = hf_spx_ndps_func_registry;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_registry_func_vals);
                    break;
                case 0x060979:
                    ndps_hfname = hf_spx_ndps_func_notify;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_notify_func_vals);
                    break;
                case 0x06097a:
                    ndps_hfname = hf_spx_ndps_func_resman;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_resman_func_vals);
                    break;
                case 0x06097b:
                    ndps_hfname = hf_spx_ndps_func_delivery;
                    ndps_func_string = match_strval(ndps_func, spx_ndps_deliver_func_vals);
                    break;
                default:
                    ndps_hfname = 0;
                    ndps_func_string = NULL;
                    break;
            }
            if(ndps_hfname != 0)
            {
                proto_tree_add_item(ndps_tree, ndps_hfname, tvb, foffset, 4, FALSE);
                if (ndps_func_string != NULL) 
                {
                    if (check_col(pinfo->cinfo, COL_INFO))
                        col_append_str(pinfo->cinfo, COL_INFO, (const gchar*) ndps_func_string);

                    foffset += 4;
                    proto_tree_add_item(ndps_tree, hf_ndps_auth_null, tvb, foffset, 16, FALSE);
                    foffset+=16;
                    dissect_ndps_request(tvb, pinfo, ndps_tree, ndps_prog, ndps_func, foffset);
                }
            }
        }
    }
}
