static void
dissect_iscsi_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, guint8 opcode, const char *opcode_str, guint32 data_segment_len, iscsi_session_t *iscsi_session, conversation_t *conversation) {

    guint original_offset = offset;
    proto_tree *ti = NULL;
    guint8 scsi_status = 0;
    gboolean S_bit=FALSE;
    guint cdb_offset = offset + 32; /
    guint end_offset = offset + tvb_length_remaining(tvb, offset);
    iscsi_conv_data_t *cdata = NULL;
    int paddedDataSegmentLength = data_segment_len;
    guint16 lun=0xffff;
    guint immediate_data_length=0;
    guint immediate_data_offset=0;
    itl_nexus_t *itl=NULL;
    guint ahs_cdb_length=0;
    guint ahs_cdb_offset=0;

    if(paddedDataSegmentLength & 3)
	paddedDataSegmentLength += 4 - (paddedDataSegmentLength & 3);

    /
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "iSCSI");

    /
    cdata=(iscsi_conv_data_t *)se_tree_lookup32(iscsi_session->itlq, tvb_get_ntohl(tvb, offset+16));
    if(!cdata){
        cdata = se_alloc (sizeof(iscsi_conv_data_t));
        cdata->itlq.lun=0xffff;
        cdata->itlq.scsi_opcode=0xffff;
	cdata->itlq.task_flags=0;
	cdata->itlq.data_length=0;
	cdata->itlq.bidir_data_length=0;
        cdata->itlq.fc_time = pinfo->fd->abs_ts;
        cdata->itlq.first_exchange_frame=0;
        cdata->itlq.last_exchange_frame=0;
        cdata->itlq.flags=0;
        cdata->itlq.alloc_len=0;
        cdata->itlq.extra_data=NULL;
        cdata->data_in_frame=0;
        cdata->data_out_frame=0;

        se_tree_insert32(iscsi_session->itlq, tvb_get_ntohl(tvb, offset+16), cdata);
    }

    if (opcode == ISCSI_OPCODE_SCSI_RESPONSE ||
	opcode == ISCSI_OPCODE_SCSI_DATA_IN) {
        scsi_status = tvb_get_guint8 (tvb, offset+3);
    }

    if ((opcode == ISCSI_OPCODE_SCSI_RESPONSE) ||
        (opcode == ISCSI_OPCODE_SCSI_DATA_IN) ||
        (opcode == ISCSI_OPCODE_SCSI_DATA_OUT)) {
        /
        switch(opcode){
        case ISCSI_OPCODE_SCSI_RESPONSE:
            cdata->itlq.last_exchange_frame=pinfo->fd->num;
            break;
        case ISCSI_OPCODE_SCSI_DATA_IN:
            /
            if(tvb_get_guint8(tvb, offset+1)&ISCSI_SCSI_DATA_FLAG_S){
                cdata->itlq.last_exchange_frame=pinfo->fd->num;
            }
            cdata->data_in_frame=pinfo->fd->num;
            break;
        case ISCSI_OPCODE_SCSI_DATA_OUT:
            cdata->data_out_frame=pinfo->fd->num;
            break;
        }

    } else if (opcode == ISCSI_OPCODE_SCSI_COMMAND) {
        /
        if(tvb_get_guint8(tvb, offset+8)&0x40){
          /
          lun=tvb_get_guint8(tvb,offset+8)&0x3f;
          lun<<=8;
          lun|=tvb_get_guint8(tvb,offset+9);
        } else {
          lun=tvb_get_guint8(tvb,offset+9);
        }

        cdata->itlq.lun=lun;
        cdata->itlq.first_exchange_frame=pinfo->fd->num;

        itl=(itl_nexus_t *)se_tree_lookup32(iscsi_session->itl, lun);
        if(!itl){
            itl=se_alloc(sizeof(itl_nexus_t));
            itl->cmdset=0xff;
            itl->conversation=conversation;
            se_tree_insert32(iscsi_session->itl, lun, itl);
        }

    }

    if(!itl){
        itl=(itl_nexus_t *)se_tree_lookup32(iscsi_session->itl, cdata->itlq.lun);
    }


    if (check_col(pinfo->cinfo, COL_INFO)) {

        if (opcode != ISCSI_OPCODE_SCSI_COMMAND) {

            col_append_str(pinfo->cinfo, COL_INFO, opcode_str);

	    if (opcode == ISCSI_OPCODE_SCSI_RESPONSE ||
		(opcode == ISCSI_OPCODE_SCSI_DATA_IN &&
		 (tvb_get_guint8(tvb, offset + 1) & ISCSI_SCSI_DATA_FLAG_S))) {
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (scsi_status, scsi_status_val, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_LOGIN_RESPONSE) {
		guint16 login_status = tvb_get_ntohs(tvb, offset+36);
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (login_status, iscsi_login_status, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_LOGOUT_COMMAND) {
		guint8 logoutReason;
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		    logoutReason = tvb_get_guint8(tvb, offset+11);
		} else if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		    logoutReason = tvb_get_guint8(tvb, offset+1) & 0x7f;
		}
		else {
		    logoutReason = tvb_get_guint8(tvb, offset+23);
		}
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (logoutReason, iscsi_logout_reasons, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_TASK_MANAGEMENT_FUNCTION) {
		guint8 tmf = tvb_get_guint8(tvb, offset + 1);
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (tmf, iscsi_task_management_functions, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_TASK_MANAGEMENT_FUNCTION_RESPONSE) {
		guint8 resp = tvb_get_guint8(tvb, offset + 2);
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (resp, iscsi_task_management_responses, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_REJECT) {
		guint8 reason = tvb_get_guint8(tvb, offset + 2);
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (reason, iscsi_reject_reasons, "0x%x"));
	    }
	    else if (opcode == ISCSI_OPCODE_ASYNC_MESSAGE) {
		guint8 asyncEvent = tvb_get_guint8(tvb, offset + 36);
		col_append_fstr (pinfo->cinfo, COL_INFO, " (%s)",
				 val_to_str (asyncEvent, iscsi_asyncevents, "0x%x"));
	    }
	}
    }

    /
    if (tree) {
	proto_item *tp;
	/
	tp = proto_tree_add_protocol_format(tree, proto_iscsi, tvb,
					    offset, -1, "iSCSI (%s)",
					    opcode_str);
	ti = proto_item_add_subtree(tp, ett_iscsi);
    }
    proto_tree_add_uint(ti, hf_iscsi_Opcode, tvb,
			    offset + 0, 1, opcode);
    if((opcode & TARGET_OPCODE_BIT) == 0) {
	    /
	    gint b = tvb_get_guint8(tvb, offset + 0);
	if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		if(opcode != ISCSI_OPCODE_SCSI_DATA_OUT &&
		   opcode != ISCSI_OPCODE_LOGOUT_COMMAND &&
		   opcode != ISCSI_OPCODE_SNACK_REQUEST)
		    proto_tree_add_boolean(ti, hf_iscsi_X, tvb, offset + 0, 1, b);
	}
	    if(opcode != ISCSI_OPCODE_SCSI_DATA_OUT &&
	       opcode != ISCSI_OPCODE_LOGIN_COMMAND &&
	       opcode != ISCSI_OPCODE_SNACK_REQUEST)
		proto_tree_add_boolean(ti, hf_iscsi_I, tvb, offset + 0, 1, b);
    }

    if(opcode == ISCSI_OPCODE_NOP_OUT) {
	    /
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_ping_data);
    } else if(opcode == ISCSI_OPCODE_NOP_IN) {
	    /
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_ping_data);
    } else if(opcode == ISCSI_OPCODE_SCSI_COMMAND) {
	    /
	    guint32 ahsLen = tvb_get_guint8(tvb, offset + 4) * 4;
    	    {
		gint b = tvb_get_guint8(tvb, offset + 1);

		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

		proto_tree_add_boolean(tt, hf_iscsi_SCSICommand_F, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSICommand_R, tvb, offset + 1, 1, b);
		if(b&0x40){
		    cdata->itlq.task_flags|=SCSI_DATA_READ;
		}
		proto_tree_add_boolean(tt, hf_iscsi_SCSICommand_W, tvb, offset + 1, 1, b);
		if(b&0x20){
		    cdata->itlq.task_flags|=SCSI_DATA_WRITE;
		}
		proto_tree_add_uint(tt, hf_iscsi_SCSICommand_Attr, tvb, offset + 1, 1, b);
	    }
	    if(iscsi_protocol_version < ISCSI_PROTOCOL_DRAFT12) {
		proto_tree_add_item(ti, hf_iscsi_SCSICommand_CRN, tvb, offset + 3, 1, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpectedDataTransferLength, tvb, offset + 20, 4, FALSE);
	    cdata->itlq.data_length=tvb_get_ntohl(tvb, offset+20);
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    if(ahsLen > 0) {
		int ahs_offset=offset+48;
		guint16 ahs_length=0;
		guint8 ahs_type=0;

		while(ahs_offset<(offset+48+ahsLen)){

		    ahs_length=tvb_get_ntohs(tvb, ahs_offset);
		    proto_tree_add_item(ti, hf_iscsi_AHS_length, tvb, ahs_offset, 2, FALSE);
		    ahs_offset+=2;

		    ahs_type=tvb_get_guint8(tvb, ahs_offset);
		    proto_tree_add_item(ti, hf_iscsi_AHS_type, tvb, ahs_offset, 1, FALSE);
		    ahs_offset++;
		
		    switch(ahs_type){
		    case 0x01: /
			/
			ahs_cdb_offset=ahs_offset+1;
			ahs_cdb_length=ahs_length-1;
			proto_tree_add_item(ti, hf_iscsi_AHS_extended_cdb, tvb, ahs_cdb_offset, ahs_cdb_length, FALSE);
			ahs_offset+=ahs_length;
			break;
		    case 0x02: /
			/
			ahs_offset++;
			/
			proto_tree_add_item(ti, hf_iscsi_AHS_read_data_length, tvb, ahs_offset, 4, FALSE);
			cdata->itlq.bidir_data_length=tvb_get_ntohl(tvb, ahs_offset);
			ahs_offset+=4;
			break;
		    default:
			proto_tree_add_item(ti, hf_iscsi_AHS_blob, tvb, ahs_offset, ahs_length, FALSE);
			ahs_offset+=ahs_length;
		    }

		    /
		    if(ahs_offset&0x0003){
			ahs_offset=(ahs_offset+3)&0xfffc;
		    }

		}

	    }
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48 + ahsLen);
	    
            immediate_data_offset=offset;
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_immediate_data);
	    immediate_data_length=offset-immediate_data_offset;
    } else if(opcode == ISCSI_OPCODE_SCSI_RESPONSE) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

		proto_tree_add_boolean(tt, hf_iscsi_SCSIResponse_o, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSIResponse_u, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSIResponse_O, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSIResponse_U, tvb, offset + 1, 1, b);
	    }
	    proto_tree_add_item(ti, hf_iscsi_SCSIResponse_Response, tvb, offset + 2, 1, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_SCSIResponse_Status, tvb, offset + 3, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version <= ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_SCSIResponse_ResidualCount, tvb, offset + 20, 4, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpDataSN, tvb, offset + 36, 4, FALSE);
	    if(iscsi_protocol_version <= ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_SCSIResponse_BidiReadResidualCount, tvb, offset + 44, 4, FALSE);
	    }
	    else {
		proto_tree_add_item(ti, hf_iscsi_SCSIResponse_BidiReadResidualCount, tvb, offset + 40, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_SCSIResponse_ResidualCount, tvb, offset + 44, 4, FALSE);
	    }
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    /
	    handleDataDigest(ti, tvb, offset, paddedDataSegmentLength);
    } else if(opcode == ISCSI_OPCODE_TASK_MANAGEMENT_FUNCTION) {
	    /
 	    proto_tree_add_item(ti, hf_iscsi_TaskManagementFunction_Function, tvb, offset + 1, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    }
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TaskManagementFunction_ReferencedTaskTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_RefCmdSN, tvb, offset + 32, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_TASK_MANAGEMENT_FUNCTION_RESPONSE) {
	    /
	    proto_tree_add_item(ti, hf_iscsi_TaskManagementFunction_Response, tvb, offset + 2, 1, FALSE);
	    if(iscsi_protocol_version <= ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version < ISCSI_PROTOCOL_DRAFT12) {
		proto_tree_add_item(ti, hf_iscsi_TaskManagementFunction_ReferencedTaskTag, tvb, offset + 20, 4, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_LOGIN_COMMAND) {
	    /
	    int digestsActive = 0;
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		    if((b & CSG_MASK) >= ISCSI_CSG_OPERATIONAL_NEGOTIATION)
			digestsActive = 1;
		}
#if 0
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);
#endif

		proto_tree_add_boolean(ti, hf_iscsi_Login_T, tvb, offset + 1, 1, b);
		if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		    proto_tree_add_boolean(ti, hf_iscsi_Login_C, tvb, offset + 1, 1, b);
		}
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		    proto_tree_add_boolean(ti, hf_iscsi_Login_X, tvb, offset + 1, 1, b);
		}
		proto_tree_add_item(ti, hf_iscsi_Login_CSG, tvb, offset + 1, 1, FALSE);

		/
		if(b&0x80){
			proto_tree_add_item(ti, hf_iscsi_Login_NSG, tvb, offset + 1, 1, FALSE);
		}
	    }
	    proto_tree_add_item(ti, hf_iscsi_VersionMax, tvb, offset + 2, 1, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_VersionMin, tvb, offset + 3, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		proto_tree_add_item(ti, hf_iscsi_CID, tvb, offset + 8, 2, FALSE);
		proto_tree_add_item(ti, hf_iscsi_ISID8, tvb, offset + 12, 2, FALSE);
	    }
	    else {
		proto_item *tf = proto_tree_add_item(ti, hf_iscsi_ISID, tvb, offset + 8, 6, FALSE);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_ISID);
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT09) {
		    proto_tree_add_item(tt, hf_iscsi_ISID_Type, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_NamingAuthority, tvb, offset + 9, 3, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_Qualifier, tvb, offset + 12, 2, FALSE);
		}
		else {
		    proto_tree_add_item(tt, hf_iscsi_ISID_t, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_a, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_b, tvb, offset + 9, 2, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_c, tvb, offset + 11, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_d, tvb, offset + 12, 2, FALSE);
		}
	    }
	    if(iscsi_protocol_version < ISCSI_PROTOCOL_DRAFT12) {
		proto_tree_add_item(ti, hf_iscsi_TSID, tvb, offset + 14, 2, FALSE);
	    }
	    else {
		proto_tree_add_item(ti, hf_iscsi_TSIH, tvb, offset + 14, 2, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT08) {
		proto_tree_add_item(ti, hf_iscsi_CID, tvb, offset + 20, 2, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    if(digestsActive){
		offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    } else {
		offset += 48;
	    }
	    offset = handleDataSegmentAsTextKeys(ti, tvb, offset, data_segment_len, end_offset, digestsActive);
    } else if(opcode == ISCSI_OPCODE_LOGIN_RESPONSE) {
	    /
	    int digestsActive = 0;
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		    if((b & CSG_MASK) >= ISCSI_CSG_OPERATIONAL_NEGOTIATION)
			digestsActive = 1;
		}
#if 0
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);
#endif

		proto_tree_add_boolean(ti, hf_iscsi_Login_T, tvb, offset + 1, 1, b);
		if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		    proto_tree_add_boolean(ti, hf_iscsi_Login_C, tvb, offset + 1, 1, b);
		}
		proto_tree_add_item(ti, hf_iscsi_Login_CSG, tvb, offset + 1, 1, FALSE);
		/
		if(b&0x80){
			proto_tree_add_item(ti, hf_iscsi_Login_NSG, tvb, offset + 1, 1, FALSE);
		}
	    }

	    proto_tree_add_item(ti, hf_iscsi_VersionMax, tvb, offset + 2, 1, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_VersionActive, tvb, offset + 3, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		proto_tree_add_item(ti, hf_iscsi_ISID8, tvb, offset + 12, 2, FALSE);
	    }
	    else {
		proto_item *tf = proto_tree_add_item(ti, hf_iscsi_ISID, tvb, offset + 8, 6, FALSE);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_ISID);
		if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT09) {
		    proto_tree_add_item(tt, hf_iscsi_ISID_Type, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_NamingAuthority, tvb, offset + 9, 3, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_Qualifier, tvb, offset + 12, 2, FALSE);
		}
		else {
		    proto_tree_add_item(tt, hf_iscsi_ISID_t, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_a, tvb, offset + 8, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_b, tvb, offset + 9, 2, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_c, tvb, offset + 11, 1, FALSE);
		    proto_tree_add_item(tt, hf_iscsi_ISID_d, tvb, offset + 12, 2, FALSE);
		}
	    }
	    if(iscsi_protocol_version < ISCSI_PROTOCOL_DRAFT12) {
		proto_tree_add_item(ti, hf_iscsi_TSID, tvb, offset + 14, 2, FALSE);
	    }
	    else {
		proto_tree_add_item(ti, hf_iscsi_TSIH, tvb, offset + 14, 2, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Login_Status, tvb, offset + 36, 2, FALSE);
	    if(digestsActive){
		offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    } else {
		offset += 48;
	    }
	    offset = handleDataSegmentAsTextKeys(ti, tvb, offset, data_segment_len, end_offset, digestsActive);
    } else if(opcode == ISCSI_OPCODE_TEXT_COMMAND) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

		proto_tree_add_boolean(tt, hf_iscsi_Text_F, tvb, offset + 1, 1, b);
		if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		    proto_tree_add_boolean(tt, hf_iscsi_Text_C, tvb, offset + 1, 1, b);
		}
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegmentAsTextKeys(ti, tvb, offset, data_segment_len, end_offset, TRUE);
    } else if(opcode == ISCSI_OPCODE_TEXT_RESPONSE) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

		proto_tree_add_boolean(tt, hf_iscsi_Text_F, tvb, offset + 1, 1, b);
		if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		    proto_tree_add_boolean(tt, hf_iscsi_Text_C, tvb, offset + 1, 1, b);
		}
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegmentAsTextKeys(ti, tvb, offset, data_segment_len, end_offset, TRUE);
    } else if(opcode == ISCSI_OPCODE_SCSI_DATA_OUT) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

		proto_tree_add_boolean(tt, hf_iscsi_SCSIData_F, tvb, offset + 1, 1, b);
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_DataSN, tvb, offset + 36, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_BufferOffset, tvb, offset + 40, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    /
	    handleDataDigest(ti, tvb, offset, paddedDataSegmentLength);
    } else if(opcode == ISCSI_OPCODE_SCSI_DATA_IN) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);

                if(b&ISCSI_SCSI_DATA_FLAG_S){
                   S_bit=TRUE;
                }
		proto_tree_add_boolean(tt, hf_iscsi_SCSIData_F, tvb, offset + 1, 1, b);
		if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT08) {
		    proto_tree_add_boolean(tt, hf_iscsi_SCSIData_A, tvb, offset + 1, 1, b);
		}
		proto_tree_add_boolean(tt, hf_iscsi_SCSIData_O, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSIData_U, tvb, offset + 1, 1, b);
		proto_tree_add_boolean(tt, hf_iscsi_SCSIData_S, tvb, offset + 1, 1, b);
	    }
	    if(S_bit){
		proto_tree_add_item(ti, hf_iscsi_SCSIResponse_Status, tvb, offset + 3, 1, FALSE);
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version <= ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_SCSIData_ResidualCount, tvb, offset + 20, 4, FALSE);
	    }
	    else {
		proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_DataSN, tvb, offset + 36, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_BufferOffset, tvb, offset + 40, 4, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_SCSIData_ResidualCount, tvb, offset + 44, 4, FALSE);
	    }
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    /
	    handleDataDigest(ti, tvb, offset, paddedDataSegmentLength);
    } else if(opcode == ISCSI_OPCODE_LOGOUT_COMMAND) {
	    /
	    if(iscsi_protocol_version >= ISCSI_PROTOCOL_DRAFT13) {
		proto_tree_add_item(ti, hf_iscsi_Logout_Reason, tvb, offset + 1, 1, FALSE);
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    }
	    if(iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08) {
		proto_tree_add_item(ti, hf_iscsi_CID, tvb, offset + 8, 2, FALSE);
		proto_tree_add_item(ti, hf_iscsi_Logout_Reason, tvb, offset + 11, 1, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT08) {
		proto_tree_add_item(ti, hf_iscsi_CID, tvb, offset + 20, 2, FALSE);
		if(iscsi_protocol_version < ISCSI_PROTOCOL_DRAFT13) {
		    proto_tree_add_item(ti, hf_iscsi_Logout_Reason, tvb, offset + 23, 1, FALSE);
		}
	    }
	    proto_tree_add_item(ti, hf_iscsi_CmdSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_LOGOUT_RESPONSE) {
	    /
	    proto_tree_add_item(ti, hf_iscsi_Logout_Response, tvb, offset + 2, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Time2Wait, tvb, offset + 40, 2, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Time2Retain, tvb, offset + 42, 2, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_SNACK_REQUEST) {
	    /
	    {
		gint b = tvb_get_guint8(tvb, offset + 1);
#if 0
		proto_item *tf = proto_tree_add_uint(ti, hf_iscsi_Flags, tvb, offset + 1, 1, b);
		proto_tree *tt = proto_item_add_subtree(tf, ett_iscsi_Flags);
#endif

		proto_tree_add_item(ti, hf_iscsi_snack_type, tvb, offset + 1, 1, b);
	    }
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
		proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    if(iscsi_protocol_version <= ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_BegRun, tvb, offset + 20, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_RunLength, tvb, offset + 24, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_ExpDataSN, tvb, offset + 36, 4, FALSE);
	    }
	    else {
		proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_ExpStatSN, tvb, offset + 28, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_BegRun, tvb, offset + 40, 4, FALSE);
		proto_tree_add_item(ti, hf_iscsi_RunLength, tvb, offset + 44, 4, FALSE);
	    }
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_R2T) {
	    /
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
		proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
		proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    }
	    proto_tree_add_item(ti, hf_iscsi_InitiatorTaskTag, tvb, offset + 16, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_TargetTransferTag, tvb, offset + 20, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_R2TSN, tvb, offset + 36, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_BufferOffset, tvb, offset + 40, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_DesiredDataLength, tvb, offset + 44, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
    } else if(opcode == ISCSI_OPCODE_ASYNC_MESSAGE) {
	    /
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_LUN, tvb, offset + 8, 8, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_AsyncEvent, tvb, offset + 36, 1, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_EventVendorCode, tvb, offset + 37, 1, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Parameter1, tvb, offset + 38, 2, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Parameter2, tvb, offset + 40, 2, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_Parameter3, tvb, offset + 42, 2, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_async_message_data);
    } else if(opcode == ISCSI_OPCODE_REJECT) {
	    /
	    proto_tree_add_item(ti, hf_iscsi_Reject_Reason, tvb, offset + 2, 1, FALSE);
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    proto_tree_add_item(ti, hf_iscsi_StatSN, tvb, offset + 24, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_ExpCmdSN, tvb, offset + 28, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_MaxCmdSN, tvb, offset + 32, 4, FALSE);
	    proto_tree_add_item(ti, hf_iscsi_DataSN, tvb, offset + 36, 4, FALSE);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_error_pdu_data);
    } else if(opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_I0 ||
		opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_I1 ||
		opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_I2 ||
		opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_T0 ||
		opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_T1 ||
		opcode == ISCSI_OPCODE_VENDOR_SPECIFIC_T2) {
	    /
	    if(iscsi_protocol_version > ISCSI_PROTOCOL_DRAFT09) {
		proto_tree_add_item(ti, hf_iscsi_TotalAHSLength, tvb, offset + 4, 1, FALSE);
	    }
	    proto_tree_add_uint(ti, hf_iscsi_DataSegmentLength, tvb, offset + 5, 3, data_segment_len);
	    offset = handleHeaderDigest(iscsi_session, ti, tvb, offset, 48);
	    offset = handleDataSegment(ti, tvb, offset, data_segment_len, end_offset, hf_iscsi_vendor_specific_data);
    }



    /
    switch(opcode){
    case ISCSI_OPCODE_SCSI_RESPONSE:
        if (cdata->itlq.first_exchange_frame){
            nstime_t delta_time;
            proto_tree_add_uint(ti, hf_iscsi_request_frame, tvb, 0, 0, cdata->itlq.first_exchange_frame);
            nstime_delta(&delta_time, &pinfo->fd->abs_ts, &cdata->itlq.fc_time);
            proto_tree_add_time(ti, hf_iscsi_time, tvb, 0, 0, &delta_time);
        }
        if (cdata->data_in_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_in_frame, tvb, 0, 0, cdata->data_in_frame);
        if (cdata->data_out_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_out_frame, tvb, 0, 0, cdata->data_out_frame);
        break;
    case ISCSI_OPCODE_SCSI_DATA_IN:
        /
        if(!S_bit){
            if (cdata->itlq.first_exchange_frame)
                proto_tree_add_uint(ti, hf_iscsi_request_frame, tvb, 0, 0, cdata->itlq.first_exchange_frame);
            if (cdata->itlq.last_exchange_frame)
                proto_tree_add_uint(ti, hf_iscsi_response_frame, tvb, 0, 0, cdata->itlq.last_exchange_frame);
        } else {
            if (cdata->itlq.first_exchange_frame){
                 nstime_t delta_time;
                 proto_tree_add_uint(ti, hf_iscsi_request_frame, tvb, 0, 0, cdata->itlq.first_exchange_frame);
		 nstime_delta(&delta_time, &pinfo->fd->abs_ts, &cdata->itlq.fc_time);
                 proto_tree_add_time(ti, hf_iscsi_time, tvb, 0, 0, &delta_time);
            }
        }
        if (cdata->data_out_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_out_frame, tvb, 0, 0, cdata->data_out_frame);
        break;
    case ISCSI_OPCODE_SCSI_DATA_OUT:
        if (cdata->itlq.first_exchange_frame)
            proto_tree_add_uint(ti, hf_iscsi_request_frame, tvb, 0, 0, cdata->itlq.first_exchange_frame);
        if (cdata->data_in_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_in_frame, tvb, 0, 0, cdata->data_in_frame);
        if (cdata->itlq.last_exchange_frame)
            proto_tree_add_uint(ti, hf_iscsi_response_frame, tvb, 0, 0, cdata->itlq.last_exchange_frame);
        break;
    case ISCSI_OPCODE_SCSI_COMMAND:
        if (cdata->data_in_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_in_frame, tvb, 0, 0, cdata->data_in_frame);
        if (cdata->data_out_frame)
            proto_tree_add_uint(ti, hf_iscsi_data_out_frame, tvb, 0, 0, cdata->data_out_frame);
        if (cdata->itlq.last_exchange_frame)
            proto_tree_add_uint(ti, hf_iscsi_response_frame, tvb, 0, 0, cdata->itlq.last_exchange_frame);
        break;
    }



    proto_item_set_len(ti, offset - original_offset);

    if((opcode & ((iscsi_protocol_version == ISCSI_PROTOCOL_DRAFT08)?
		  ~(X_BIT | I_BIT) :
		  ~I_BIT)) == ISCSI_OPCODE_SCSI_COMMAND) {
	tvbuff_t *cdb_tvb, *data_tvb;
	int tvb_len, tvb_rlen;
	guint8 scsi_opcode;

        /
	tvb_len=tvb_length_remaining(tvb, cdb_offset);
	tvb_rlen=tvb_reported_length_remaining(tvb, cdb_offset);
	scsi_opcode=tvb_get_guint8(tvb, cdb_offset);
	if(ahs_cdb_length && ahs_cdb_length<1024){
		char *cdb_buf;

		/
		cdb_buf=ep_alloc(16+ahs_cdb_length);
		/
		tvb_memcpy(tvb, cdb_buf, cdb_offset, 16);
		/
		tvb_memcpy(tvb, cdb_buf+16, ahs_cdb_offset, ahs_cdb_length);

		cdb_tvb = tvb_new_real_data(cdb_buf,
					  ahs_cdb_length+16,
					  ahs_cdb_length+16);

		tvb_set_child_real_data_tvbuff(tvb, cdb_tvb);

		add_new_data_source(pinfo, cdb_tvb, "CDB+AHS");
	} else {
		if(tvb_len>16){
		    tvb_len=16;
		}
		if(tvb_rlen>16){
		    tvb_rlen=16;
		}
		cdb_tvb=tvb_new_subset(tvb, cdb_offset, tvb_len, tvb_rlen);
	}
        dissect_scsi_cdb(cdb_tvb, pinfo, tree, SCSI_DEV_UNKNOWN, &cdata->itlq, itl);
	/
	if (check_col(pinfo->cinfo, COL_INFO)) {
	    col_set_fence(pinfo->cinfo, COL_INFO);
	}
	/
	if(immediate_data_length){
            /
	    tvb_len=tvb_length_remaining(tvb, immediate_data_offset);
	    if(tvb_len>(int)immediate_data_length)
	        tvb_len=immediate_data_length;
	    tvb_rlen=tvb_reported_length_remaining(tvb, immediate_data_offset);
    	    if(tvb_rlen>(int)immediate_data_length)
	        tvb_rlen=immediate_data_length;
	    data_tvb=tvb_new_subset(tvb, immediate_data_offset, tvb_len, tvb_rlen);
            dissect_scsi_payload (data_tvb, pinfo, tree,
		  	          TRUE,
			          &cdata->itlq, itl);
	}
    }
    else if (opcode == ISCSI_OPCODE_SCSI_RESPONSE) {
        if (scsi_status == 0x2) {
            /
            /
	    if((end_offset - offset) >= 2) {
		int senseLen = tvb_get_ntohs(tvb, offset);
		if(ti != NULL)
		    proto_tree_add_item(ti, hf_iscsi_SenseLength, tvb, offset, 2, FALSE);
		offset += 2;
		if(senseLen > 0){
		    tvbuff_t *data_tvb;
		    int tvb_len, tvb_rlen;

		    tvb_len=tvb_length_remaining(tvb, offset);
		    if(tvb_len>senseLen)
			tvb_len=senseLen;
		    tvb_rlen=tvb_reported_length_remaining(tvb, offset);
		    if(tvb_rlen>senseLen)
			tvb_rlen=senseLen;
		    data_tvb=tvb_new_subset(tvb, offset, tvb_len, tvb_rlen);
		    dissect_scsi_snsinfo (data_tvb, pinfo, tree, 0,
					  tvb_len,
					  &cdata->itlq, itl);
		}
	    }
        }
        else {
            dissect_scsi_rsp(tvb, pinfo, tree, &cdata->itlq, itl, scsi_status);
        }
    }
    else if ((opcode == ISCSI_OPCODE_SCSI_DATA_IN) ||
             (opcode == ISCSI_OPCODE_SCSI_DATA_OUT)) {
	tvbuff_t *data_tvb;
	int tvb_len, tvb_rlen;

        /
	tvb_len=tvb_length_remaining(tvb, offset);
	if(tvb_len>(int)data_segment_len)
	    tvb_len=data_segment_len;
	tvb_rlen=tvb_reported_length_remaining(tvb, offset);
	if(tvb_rlen>(int)data_segment_len)
	    tvb_rlen=data_segment_len;
	data_tvb=tvb_new_subset(tvb, offset, tvb_len, tvb_rlen);
        dissect_scsi_payload (data_tvb, pinfo, tree,
			      (opcode==ISCSI_OPCODE_SCSI_DATA_OUT),
			      &cdata->itlq, itl);
    }

    if(S_bit){
        dissect_scsi_rsp(tvb, pinfo, tree, &cdata->itlq, itl, scsi_status);
    }
}
