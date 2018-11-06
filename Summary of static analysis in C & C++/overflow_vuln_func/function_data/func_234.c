void
dissect_scsi_payload (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                      gboolean isreq, itlq_nexus_t *itlq, itl_nexus_t *itl)
{
    int offset=0;
    proto_item *ti;
    proto_tree *scsi_tree = NULL;
    guint8 opcode = 0xFF;
    scsi_device_type devtype=0xff;
    scsi_task_data_t *cdata = NULL;
    int payload_len;
    const char *old_proto;
    cmdset_t *csdata;

    if(!itlq || !itl){
        /
	proto_tree_add_text(tree, tvb, offset, 0, "Unknown SCSI exchange, can not decode SCSI data");
        return;
    }

    payload_len=tvb_length(tvb);
    cdata = ep_alloc(sizeof(scsi_task_data_t));
    cdata->itl=itl;
    cdata->itlq=itlq;
    cdata->type=SCSI_PDU_TYPE_CDB;
    tap_queue_packet(scsi_tap, pinfo, cdata);

    csdata=get_cmdset_data(itlq, itl);

    old_proto=pinfo->current_proto;
    pinfo->current_proto="SCSI";

    opcode = cdata->itlq->scsi_opcode;
    devtype = cdata->itl->cmdset&SCSI_CMDSET_MASK;

    if (tree) {
        ti = proto_tree_add_protocol_format (tree, proto_scsi, tvb, offset,
                                             payload_len,
                                             "SCSI Payload (%s %s)",
                                             val_to_str (opcode,
                                                         csdata->cdb_vals,
                                                         "CDB:0x%02x"),
                                             isreq ? "Request Data" : "Response Data");
        scsi_tree = proto_item_add_subtree (ti, ett_scsi);
    }

    if (check_col (pinfo->cinfo, COL_INFO)) {
	col_add_fstr (pinfo->cinfo, COL_INFO,
		"SCSI: Data %s LUN: 0x%02x (%s %s) ",
		isreq ? "Out" : "In",
		itlq->lun,
		val_to_str (opcode, csdata->cdb_vals, "0x%02x"),
		isreq ? "Request Data" : "Response Data");

	col_set_fence(pinfo->cinfo, COL_INFO);
    }


    ti=proto_tree_add_uint(scsi_tree, hf_scsi_lun, tvb, 0, 0, itlq->lun);
    PROTO_ITEM_SET_GENERATED(ti);

    if(itl){
        ti=proto_tree_add_uint_format(scsi_tree, hf_scsi_inq_devtype, tvb, 0, 0, itl->cmdset&SCSI_CMDSET_MASK, "Command Set:%s (0x%02x) %s", val_to_str(itl->cmdset&SCSI_CMDSET_MASK, scsi_devtype_val, "Unknown"), itl->cmdset&SCSI_CMDSET_MASK,itl->cmdset&SCSI_CMDSET_DEFAULT?"(Using default commandset)":"");
        PROTO_ITEM_SET_GENERATED(ti);

        if(itlq && itlq->scsi_opcode!=0xffff){
            ti=proto_tree_add_uint(scsi_tree, csdata->hf_opcode, tvb, 0, 0, itlq->scsi_opcode);
            PROTO_ITEM_SET_GENERATED(ti);
        }
    }

    if(itlq->first_exchange_frame){
        ti=proto_tree_add_uint(scsi_tree, hf_scsi_request_frame, tvb, 0, 0, itlq->first_exchange_frame);
        PROTO_ITEM_SET_GENERATED(ti);
    }

    if(itlq->last_exchange_frame){
        ti=proto_tree_add_uint(scsi_tree, hf_scsi_response_frame, tvb, 0, 0, itlq->last_exchange_frame);
        PROTO_ITEM_SET_GENERATED(ti);
    }


    if (tree == NULL) {
        /
        if (opcode == SCSI_SPC2_INQUIRY) {
            dissect_spc3_inquiry (tvb, pinfo, scsi_tree, offset, isreq,
                                  FALSE, payload_len, cdata);
        }
    } else {
        /
        if(csdata->cdb_table && (csdata->cdb_table)[opcode].func){
            (csdata->cdb_table)[opcode].func(tvb, pinfo, scsi_tree, offset,
                               isreq, FALSE, payload_len, cdata);
        } else if(spc[opcode].func){
            spc[opcode].func(tvb, pinfo, scsi_tree, offset,
                               isreq, FALSE, payload_len, cdata);
        } else { /
            call_dissector (data_handle, tvb, pinfo, scsi_tree);
        }
    }

    pinfo->current_proto=old_proto;
}
