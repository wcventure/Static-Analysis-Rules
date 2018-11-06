static void
dissect_fcp_rsp (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    guint32 offset = 0,
        del_usecs = 0;
    guint len = 0,
          add_len = 0,
          rsplen = 0;
    gchar str[128];
    guint8 flags;
    proto_item *ti;
    proto_tree *fcp_tree;
    guint8 status;
    conversation_t *conversation;
    fcp_conv_data_t *cdata = NULL;
    fcp_conv_key_t ckey;
    scsi_task_id_t task_key;

    status = tvb_get_guint8 (tvb, offset+11);
    
    if (check_col (pinfo->cinfo, COL_INFO)) {
        col_append_fstr (pinfo->cinfo, COL_INFO, " , %s",
                         val_to_str (status, scsi_status_val, "0x%x"));
    }

    /
    conversation = find_conversation (pinfo->fd->num, &pinfo->src, &pinfo->dst,
                                      pinfo->ptype, pinfo->oxid,
                                      pinfo->rxid, NO_PORT2);
    if (conversation) {
        ckey.conv_idx = conversation->index;
    
        cdata = (fcp_conv_data_t *)g_hash_table_lookup (fcp_req_hash,
                                                        &ckey);
        task_key.conv_id = task_key.task_id = conversation->index;
        pinfo->private_data = (void *)&task_key;
    }
    
    if (tree) {
        /
        len = FCP_DEF_RSP_LEN;

        flags = tvb_get_guint8 (tvb, offset+10);
        if (flags & 0x2) {
            add_len = tvb_get_ntohl (tvb, offset+20);
            len += add_len;
        }
        if (flags & 0x1) {
            add_len = tvb_get_ntohl (tvb, offset+16);
            len += add_len;
        }

        /
        if ((gint) len <= 0) {
            proto_tree_add_text(tree, tvb, 0, 0, "Invalid length: %u", len);
            return;
        }

        ti = proto_tree_add_protocol_format (tree, proto_fcp, tvb, 0, len,
                                             "FCP_RSP");
        fcp_tree = proto_item_add_subtree (ti, ett_fcp);
        proto_tree_add_uint_hidden (fcp_tree, hf_fcp_type, tvb, offset, 0, 0);

        if (cdata) {
            del_usecs = (pinfo->fd->abs_secs - cdata->abs_secs)* 1000000 +
                (pinfo->fd->abs_usecs - cdata->abs_usecs);
            if (del_usecs > 1000)
                proto_tree_add_text (fcp_tree, tvb, offset, 0,
                                     "Cmd Response Time: %d msecs",
                                     del_usecs/1000);
            else
                proto_tree_add_text (fcp_tree, tvb, offset, 0,
                                     "Cmd Response Time: %d usecs",
                                     del_usecs);
            if (cdata->fcp_lun >= 0)
                proto_tree_add_uint_hidden (fcp_tree, hf_fcp_singlelun, tvb,
                                            offset, 0, cdata->fcp_lun);
        }
        proto_tree_add_uint_format (fcp_tree, hf_fcp_rspflags, tvb, offset+10,
                                    1, flags, "Flags: 0x%02x (%s)", flags,
                                    rspflags_to_str (flags, str));
        proto_tree_add_item (fcp_tree, hf_fcp_scsistatus, tvb, offset+11, 1, 0);
        if (flags & 0xC)
            proto_tree_add_item (fcp_tree, hf_fcp_resid, tvb, offset+12, 4, 0);
        if (flags & 0x2)
            proto_tree_add_item (fcp_tree, hf_fcp_snslen, tvb, offset+16, 4, 0);
        if (flags & 0x1) {
            rsplen = tvb_get_ntohl (tvb, offset+20);
            proto_tree_add_item (fcp_tree, hf_fcp_rsplen, tvb, offset+20, 4, 0);
            proto_tree_add_item (fcp_tree, hf_fcp_rspcode, tvb, offset+27, 1,
                                 0);
        }
        if (flags & 0x2) {
            dissect_scsi_snsinfo (tvb, pinfo, tree, offset+24+rsplen,
                                  tvb_get_ntohl (tvb, offset+16), 
				  (guint16) (cdata?cdata->fcp_lun:0xffff) );
        }
        if (cdata) {
            g_mem_chunk_free (fcp_req_vals, cdata);
            g_hash_table_remove (fcp_req_hash, &ckey);
        }
    }
}
