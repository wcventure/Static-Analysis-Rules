static void
dissect_q931_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    gboolean is_tpkt)
{
	int		offset = 0;
	proto_tree	*q931_tree = NULL;
	proto_tree	*ie_tree = NULL;
	proto_item	*ti, *ti_ie;
	guint8		call_ref_len;
	guint8		call_ref[15];
	guint32		call_ref_val;
	guint8		message_type, segmented_message_type;
	guint8		info_element;
	guint16		info_element_len;
	gboolean	more_frags; 
	guint32		frag_len;
	fragment_data *fd_head;
	tvbuff_t *next_tvb = NULL;

	pi_current++;
	if(pi_current==5){
		pi_current=0;
	}
	q931_pi=&pi_arr[pi_current];
	/
	reset_q931_packet_info(q931_pi);
	have_valid_q931_pi=TRUE;

	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "Q.931");

	if (tree) {
		ti = proto_tree_add_item(tree, proto_q931, tvb, offset, -1,
		    FALSE);
		q931_tree = proto_item_add_subtree(ti, ett_q931);

		dissect_q931_protocol_discriminator(tvb, offset, q931_tree);
	}
	offset += 1;
	call_ref_len = tvb_get_guint8(tvb, offset) & 0xF;	/
	if (q931_tree != NULL)
		proto_tree_add_uint(q931_tree, hf_q931_call_ref_len, tvb, offset, 1, call_ref_len);
	offset += 1;
	switch (call_ref_len) {
		case 0: call_ref_val = 0; break;
		case 1:	call_ref_val = tvb_get_guint8(tvb, offset);	break;
		case 2:	call_ref_val = tvb_get_ntohs(tvb, offset); break;
		case 3:	call_ref_val = tvb_get_ntoh24(tvb, offset); break;
		default: call_ref_val = tvb_get_ntohl(tvb, offset);
	} 
	if (call_ref_len != 0) {
		tvb_memcpy(tvb, call_ref, offset, call_ref_len);
		if (q931_tree != NULL) {
			proto_tree_add_boolean(q931_tree, hf_q931_call_ref_flag,
			    tvb, offset, 1, (call_ref[0] & 0x80) != 0);
			call_ref[0] &= 0x7F;
			proto_tree_add_bytes(q931_tree, hf_q931_call_ref,
			    tvb, offset, call_ref_len, call_ref);
		} else
		{       /
			call_ref[0] &= 0x7F;
		}
		g_memmove(&(q931_pi->crv), call_ref, call_ref_len);
		offset += call_ref_len;
	}
	message_type = tvb_get_guint8(tvb, offset);
	if (check_col(pinfo->cinfo, COL_INFO)) {
		col_add_str(pinfo->cinfo, COL_INFO,
		    val_to_str(message_type, q931_message_type_vals,
		      "Unknown message type (0x%02X)"));
	}
	if (q931_tree != NULL)
		proto_tree_add_uint(q931_tree, hf_q931_message_type, tvb, offset, 1, message_type);
	offset += 1;

	/
	if ((message_type != Q931_SEGMENT) || !q931_reassembly || 
			(tvb_reported_length_remaining(tvb, offset) <= 4)) {
		dissect_q931_IEs(tvb, pinfo, tree, q931_tree, is_tpkt, offset, 0);
		return;
	}
	info_element = tvb_get_guint8(tvb, offset);
	info_element_len = tvb_get_guint8(tvb, offset + 1);
	if ((info_element != Q931_IE_SEGMENTED_MESSAGE) || (info_element_len < 2)) {
		dissect_q931_IEs(tvb, pinfo, tree, q931_tree, is_tpkt, offset, 0);
		return;
	}
	/
	ti_ie = proto_tree_add_text(q931_tree, tvb, offset, 1+1+info_element_len, "%s",
				    val_to_str(info_element, q931_info_element_vals[0], "Unknown information element (0x%02X)"));
	ie_tree = proto_item_add_subtree(ti_ie, ett_q931_ie);
	proto_tree_add_text(ie_tree, tvb, offset, 1, "Information element: %s",
				    val_to_str(info_element, q931_info_element_vals[0], "Unknown (0x%02X)"));
	proto_tree_add_text(ie_tree, tvb, offset + 1, 1, "Length: %u", info_element_len);
	dissect_q931_segmented_message_ie(tvb, offset + 2, info_element_len, ie_tree);
	more_frags = (tvb_get_guint8(tvb, offset + 2) & 0x7F) != 0;
	segmented_message_type = tvb_get_guint8(tvb, offset + 3);
	if (check_col(pinfo->cinfo, COL_INFO)) {
		col_append_fstr(pinfo->cinfo, COL_INFO, " of %s",
		    val_to_str(segmented_message_type, q931_message_type_vals, "Unknown message type (0x%02X)"));
	}
	offset += 1 + 1 + info_element_len;
	/
	frag_len = tvb_length_remaining(tvb, offset);
	fd_head = fragment_add_seq_next(tvb, offset, pinfo, call_ref_val,
									q931_fragment_table, q931_reassembled_table,
									frag_len, more_frags);
	if (fd_head) {
		if (pinfo->fd->num == fd_head->reassembled_in) {  /
			if (fd_head->next != NULL) {  /
				next_tvb = tvb_new_real_data(fd_head->data, fd_head->len, fd_head->len);
				tvb_set_child_real_data_tvbuff(tvb, next_tvb);
				add_new_data_source(pinfo, next_tvb, "Reassembled Q.931 IEs");
				/
				if (tree) show_fragment_seq_tree(fd_head, &q931_frag_items, q931_tree, pinfo, next_tvb);
			} else {  /
				next_tvb = tvb_new_subset(tvb, offset, -1, -1);
			}
			if (check_col(pinfo->cinfo, COL_INFO)) {
				col_add_fstr(pinfo->cinfo, COL_INFO, "%s [reassembled]",
				    val_to_str(segmented_message_type, q931_message_type_vals, "Unknown message type (0x%02X)"));
			}
		} else {
			if (tree) proto_tree_add_uint(q931_tree, hf_q931_reassembled_in, tvb, offset, frag_len, fd_head->reassembled_in);
		}
	}
	if (next_tvb)
		dissect_q931_IEs(next_tvb, pinfo, tree, q931_tree, is_tpkt, 0, 0);
}
