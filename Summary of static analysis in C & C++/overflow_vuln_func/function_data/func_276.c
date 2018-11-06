static void
dissect_fcp_cmnd (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    int offset = 0;
    int len,
        add_len = 0;
    gchar str[128];
    guint8 flags, lun0;
    proto_item *ti;
    proto_tree *fcp_tree = NULL;
    conversation_t *conversation;
    fcp_conv_data_t *cdata;
    fcp_conv_key_t ckey, *req_key;
    scsi_task_id_t task_key;
    guint16 lun=0xffff;

    /
    flags = tvb_get_guint8 (tvb, offset+10);
    if (flags) {
        add_len = tvb_get_guint8 (tvb, offset+11) & 0x7C;
        add_len = add_len >> 2;
        
        len = FCP_DEF_CMND_LEN + add_len;
    }
    else {
        len = FCP_DEF_CMND_LEN;
    }

    /
    /
    /
    
    conversation = find_conversation (pinfo->fd->num, &pinfo->src, &pinfo->dst,
                                      pinfo->ptype, pinfo->oxid,
                                      pinfo->rxid, NO_PORT2);
    if (!conversation) {
        conversation = conversation_new (pinfo->fd->num, &pinfo->src, &pinfo->dst,
                                         pinfo->ptype, pinfo->oxid,
                                         pinfo->rxid, NO_PORT2);
    }
    
    ckey.conv_idx = conversation->index;
    task_key.conv_id = conversation->index;
    task_key.task_id = conversation->index;
    pinfo->private_data = (void *)&task_key;
    
    cdata = (fcp_conv_data_t *)g_hash_table_lookup (fcp_req_hash,
                                                    &ckey);
    if (cdata) {
        /
        cdata->fcp_dl = tvb_get_ntohl (tvb, offset+12+16+add_len);
        cdata->abs_usecs = pinfo->fd->abs_usecs;
        cdata->abs_secs = pinfo->fd->abs_secs;
    }
    else {
        req_key = g_mem_chunk_alloc (fcp_req_keys);
        req_key->conv_idx = conversation->index;
        
        cdata = g_mem_chunk_alloc (fcp_req_vals);
        cdata->fcp_dl = tvb_get_ntohl (tvb, offset+12+16+add_len);
        cdata->abs_usecs = pinfo->fd->abs_usecs;
        cdata->abs_secs = pinfo->fd->abs_secs;
        
        g_hash_table_insert (fcp_req_hash, req_key, cdata);
    }
    
    /

    if (tree) {
        /
        if ((gint) len <= 0) {
            proto_tree_add_text(tree, tvb, 0, 0, "Invalid length: %u", len);
            return;
        }

        ti = proto_tree_add_protocol_format (tree, proto_fcp, tvb, 0, len,
                                             "FCP_CMND");
        fcp_tree = proto_item_add_subtree (ti, ett_fcp);
    }    
    proto_tree_add_uint_hidden (fcp_tree, hf_fcp_type, tvb, offset, 0, 0);
    
    lun0 = tvb_get_guint8 (tvb, offset);
    
    /
    /
    if (lun0) {
      cdata->fcp_lun = -1;
      proto_tree_add_item (fcp_tree, hf_fcp_multilun, tvb, offset, 8, 0);
      lun=tvb_get_guint8(tvb, offset)&0x3f;
      lun<<=8;
      lun|=tvb_get_guint8(tvb, offset+1);
    }
    else {
      cdata->fcp_lun = tvb_get_guint8 (tvb, offset+1);
      proto_tree_add_item (fcp_tree, hf_fcp_singlelun, tvb, offset+1,
			   1, 0);
      lun=tvb_get_guint8(tvb, offset+1);
    }

    proto_tree_add_item (fcp_tree, hf_fcp_crn, tvb, offset+8, 1, 0);
    proto_tree_add_item (fcp_tree, hf_fcp_taskattr, tvb, offset+9, 1, 0);
    proto_tree_add_uint_format (fcp_tree, hf_fcp_taskmgmt, tvb, offset+10,
				1, flags,
				"Task Management Flags: 0x%x (%s)",
				flags,
				task_mgmt_flags_to_str (flags, str));
    proto_tree_add_item (fcp_tree, hf_fcp_addlcdblen, tvb, offset+11, 1, 0);
    proto_tree_add_item (fcp_tree, hf_fcp_rddata, tvb, offset+11, 1, 0);
    proto_tree_add_item (fcp_tree, hf_fcp_wrdata, tvb, offset+11, 1, 0);
    dissect_scsi_cdb (tvb, pinfo, tree, offset+12, 16+add_len,
		      SCSI_DEV_UNKNOWN, lun);
    proto_tree_add_item (fcp_tree, hf_fcp_dl, tvb, offset+12+16+add_len,
			 4, 0);
}
