static void
dissect_isup_application_transport_parameter(tvbuff_t *parameter_tvb, packet_info *pinfo, proto_tree *parameter_tree, proto_item *parameter_item)
{ 

	guint8 application_transport_instruction_ind;
	guint8 si_and_apm_seg_ind;
	guint8 apm_Segmentation_local_ref = 0;
	guint16 aci16;
	gint offset = 0;
	guint8 octet;
	guint length = tvb_reported_length(parameter_tvb);

	gboolean more_frag;
 	gboolean   save_fragmented;
	tvbuff_t* new_tvb = NULL;
	tvbuff_t* next_tvb = NULL;
	fragment_data *frag_msg = NULL;
	 
	proto_tree_add_text(parameter_tree, parameter_tvb, offset, -1, "Application transport parameter fields:");
	proto_item_set_text(parameter_item, "Application transport, (%u byte%s length)", length , plurality(length, "", "s"));
	aci16 = tvb_get_guint8(parameter_tvb, offset);
 
	if ( (aci16 & H_8BIT_MASK) == 0x80) {
		/
		aci16 = aci16 & 0x7f;
		proto_tree_add_item( parameter_tree, hf_isup_extension_ind, parameter_tvb, offset, 1, FALSE );
		proto_tree_add_uint(parameter_tree, hf_isup_app_cont_ident , parameter_tvb, offset, 1, aci16);
		offset = offset + 1;
		}
	/
	else {
		aci16 = (aci16<<8) | (tvb_get_guint8(parameter_tvb, offset) & 0x7f);
		proto_tree_add_uint(parameter_tree, hf_isup_app_cont_ident , parameter_tvb, offset, 2, aci16);
		offset = offset + 2;
	}

	/
	proto_tree_add_text(parameter_tree, parameter_tvb, offset, -1, "Application transport instruction indicators: ");
	application_transport_instruction_ind = tvb_get_guint8(parameter_tvb, offset);	
	proto_tree_add_item( parameter_tree, hf_isup_extension_ind, parameter_tvb, offset, 1, FALSE );
	proto_tree_add_item( parameter_tree, hf_isup_app_Send_notification_ind, parameter_tvb, offset, 1, FALSE );
	proto_tree_add_item( parameter_tree, hf_isup_app_Release_call_ind, parameter_tvb, offset, 1, FALSE );
	offset = offset + 1; 

	/
	proto_tree_add_text(parameter_tree, parameter_tvb, offset, 1, "APM segmentation indicator:");
	si_and_apm_seg_ind  = tvb_get_guint8(parameter_tvb, offset);
	proto_tree_add_item( parameter_tree, hf_isup_extension_ind, parameter_tvb, offset, 1, FALSE );
	proto_tree_add_item( parameter_tree, hf_isup_apm_si_ind, parameter_tvb, offset, 1, FALSE );
	proto_tree_add_item( parameter_tree, hf_isup_apm_segmentation_ind, parameter_tvb, offset, 1, FALSE );
	offset = offset + 1;

	/
	if ( (si_and_apm_seg_ind & H_8BIT_MASK) == 0x00) {
		apm_Segmentation_local_ref  = tvb_get_guint8(parameter_tvb, offset);
		proto_tree_add_item( parameter_tree, hf_isup_extension_ind, parameter_tvb, offset, 1, FALSE );
		proto_tree_add_item( parameter_tree, hf_isup_apm_slr, parameter_tvb, offset, 1, FALSE );
		offset = offset + 1;
	}
	/
	if (aci16 > 3) {
		/
		octet = tvb_get_guint8(parameter_tvb,offset);
		proto_tree_add_item( parameter_tree, hf_isup_orig_addr_len, parameter_tvb, offset, 1, FALSE );
		offset++;
		if ( octet != 0){
			/
			proto_tree_add_item( parameter_tree, hf_isup_odd_even_indicator, parameter_tvb, offset, 1, FALSE );
			/
			offset++;
			proto_tree_add_item( parameter_tree, hf_isup_inn_indicator, parameter_tvb, offset, 1, FALSE );
			proto_tree_add_item( parameter_tree, hf_isup_numbering_plan_indicator, parameter_tvb, offset, 1, FALSE );
			offset++;
			/
			proto_tree_add_text(parameter_tree, parameter_tvb, offset, octet - 2, "Address digits");
			offset = offset + octet - 2;
		}
		/
		octet = tvb_get_guint8(parameter_tvb,offset);
		proto_tree_add_item( parameter_tree, hf_isup_dest_addr_len, parameter_tvb, offset, 1, FALSE );
		offset++;
		if ( octet != 0){
			/
			proto_tree_add_item( parameter_tree, hf_isup_odd_even_indicator, parameter_tvb, offset, 1, FALSE );
			/
			offset++;
			proto_tree_add_item( parameter_tree, hf_isup_inn_indicator, parameter_tvb, offset, 1, FALSE );
			proto_tree_add_item( parameter_tree, hf_isup_numbering_plan_indicator, parameter_tvb, offset, 1, FALSE );
			offset++;
			/
			proto_tree_add_text(parameter_tree, parameter_tvb, offset, octet - 2, "Address digits");
			offset = offset + octet - 2;
		}
	}
	/
	if (isup_apm_desegment &&
	    (si_and_apm_seg_ind != 0xc0 || si_and_apm_seg_ind & H_8BIT_MASK)){
		/
		save_fragmented = pinfo->fragmented;
		pinfo->fragmented = TRUE;
		more_frag = TRUE;
		if ((si_and_apm_seg_ind == 0))
			more_frag = FALSE; 
			
		frag_msg = fragment_add_seq_next(parameter_tvb, offset, pinfo,
				(apm_Segmentation_local_ref & 0x7f),			/  
				isup_apm_msg_fragment_table,					/
				isup_apm_msg_reassembled_table,					/
				tvb_length_remaining(parameter_tvb, offset),	/
				more_frag);										/

		if ((si_and_apm_seg_ind & 0x3f) !=0 && (si_and_apm_seg_ind &0x40) !=0){
			/
			fragment_set_tot_len(pinfo, apm_Segmentation_local_ref & 0x7f, isup_apm_msg_fragment_table, (si_and_apm_seg_ind & 0x3f));
		}

		new_tvb = process_reassembled_data(parameter_tvb, offset, pinfo,
			"Reassembled Message", frag_msg, &isup_apm_msg_frag_items,
			NULL, parameter_tree);

		if (frag_msg) { /
			if (check_col(pinfo->cinfo, COL_INFO))
				col_append_str(pinfo->cinfo, COL_INFO, 
				" (Message Reassembled)");
		} else { /
			if (check_col(pinfo->cinfo, COL_INFO))
				col_append_fstr(pinfo->cinfo, COL_INFO,
				" (Message fragment )");
		}
		
		pinfo->fragmented = save_fragmented;
	}/

	if ( offset == (gint)length){
		/
		proto_tree_add_text(parameter_tree, parameter_tvb, offset, 0, "Empty APM-user information field"  );
		return;
	}
	if (new_tvb) { /
		next_tvb = new_tvb;
	} else { /
	 	next_tvb = tvb_new_subset(parameter_tvb, offset, -1, -1);
	}

	proto_tree_add_text(parameter_tree, parameter_tvb, offset, -1, 
		"APM-user information field (%u Bytes)",tvb_length_remaining(parameter_tvb, offset));
	
	/ 
	if ((aci16 & 0x7fff) != 5) {
		proto_tree_add_text(parameter_tree, parameter_tvb, offset, -1, "No further dissection of APM-user information field");
		return;
	}

 	dissect_bat_ase_Encapsulated_Application_Information(next_tvb, pinfo, parameter_tree, 0);
}
