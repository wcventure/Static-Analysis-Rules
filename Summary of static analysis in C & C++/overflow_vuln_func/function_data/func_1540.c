static int
dissect_q2931(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
	int	    offset     = 0;
	proto_tree *q2931_tree = NULL;
	proto_item *ti;
	guint8	    call_ref_len;
	guint8	    call_ref[15];
	guint8	    message_type;
	guint8	    message_type_ext;
	guint16	    message_len;
	guint8	    info_element;
	guint8	    info_element_ext;
	guint16	    info_element_len;
#if 0
	int	    codeset;
	gboolean    non_locking_shift;
#endif
	static const int * ext_flags[] = {
		&hf_q2931_message_flag,
		NULL
	};
	static const int * ext_flags_follow_inst[] = {
		&hf_q2931_message_flag,
		&hf_q2931_message_action_indicator,
		NULL
	};

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "Q.2931");

	if (tree) {
		ti = proto_tree_add_item(tree, proto_q2931, tvb, offset, -1,
		    ENC_NA);
		q2931_tree = proto_item_add_subtree(ti, ett_q2931);

		proto_tree_add_uint(q2931_tree, hf_q2931_discriminator, tvb, offset, 1, tvb_get_guint8(tvb, offset));
	}
	offset += 1;
	call_ref_len = tvb_get_guint8(tvb, offset) & 0xF;	/
	if (q2931_tree != NULL)
		proto_tree_add_uint(q2931_tree, hf_q2931_call_ref_len, tvb, offset, 1, call_ref_len);
	offset += 1;
	if (call_ref_len != 0) {
		tvb_memcpy(tvb, call_ref, offset, call_ref_len);
		proto_tree_add_boolean(q2931_tree, hf_q2931_call_ref_flag,
			tvb, offset, 1, (call_ref[0] & 0x80) != 0);
		call_ref[0] &= 0x7F;
		proto_tree_add_bytes(q2931_tree, hf_q2931_call_ref, tvb, offset, call_ref_len, call_ref);
		offset += call_ref_len;
	}
	message_type = tvb_get_guint8(tvb, offset);
	col_add_str(pinfo->cinfo, COL_INFO,
		    val_to_str_ext(message_type, &q2931_message_type_vals_ext,
		      "Unknown message type (0x%02X)"));

	proto_tree_add_uint(q2931_tree, hf_q2931_message_type, tvb, offset, 1, message_type);
	offset += 1;

	message_type_ext = tvb_get_guint8(tvb, offset);
	if (message_type_ext & Q2931_MSG_TYPE_EXT_FOLLOW_INST) {
		proto_tree_add_bitmask(q2931_tree, tvb, offset, hf_q2931_message_type_ext, ett_q2931_ext, ext_flags_follow_inst, ENC_NA);
	} else {
		proto_tree_add_bitmask(q2931_tree, tvb, offset, hf_q2931_message_type_ext, ett_q2931_ext, ext_flags, ENC_NA);
	}
	offset += 1;

	message_len = tvb_get_ntohs(tvb, offset);
	if (q2931_tree != NULL)
		proto_tree_add_uint(q2931_tree, hf_q2931_message_len, tvb, offset, 2, message_len);
	offset += 2;

	/
#if 0
	codeset = 0;	/
	non_locking_shift = TRUE;
#endif
	while (tvb_reported_length_remaining(tvb, offset) > 0) {
		info_element = tvb_get_guint8(tvb, offset);
		info_element_ext = tvb_get_guint8(tvb, offset + 1);
		info_element_len = tvb_get_ntohs(tvb, offset + 2);
		if (q2931_tree != NULL) {
			dissect_q2931_ie(tvb, pinfo, offset, info_element_len,
			    q2931_tree, info_element, info_element_ext);
		}
#if 0 /
		if (non_locking_shift)
			codeset = 0;
		/
		switch (info_element) {

		case Q2931_IE_BBAND_LOCKING_SHIFT:
			if (info_element_len >= 1) {
				non_locking_shift = FALSE;
				codeset = tvb_get_guint8(tvb, offset + 4) & 0x07;
			}
			break;

		case Q2931_IE_BBAND_NLOCKING_SHIFT:
			if (info_element_len >= 1) {
				non_locking_shift = TRUE;
				codeset = tvb_get_guint8(tvb, offset + 4) & 0x07;
			}
			break;
		}
#endif
		offset += 1 + 1 + 2 + info_element_len;
	}
	return tvb_captured_length(tvb);
}
