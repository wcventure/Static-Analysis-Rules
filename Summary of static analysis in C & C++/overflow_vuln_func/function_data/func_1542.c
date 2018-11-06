static int
dissect_q933(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
	int		offset = 0;
	proto_tree	*q933_tree = NULL;
	proto_item	*ti;
	proto_tree	*ie_tree = NULL;
	guint8		call_ref_len;
	guint8		call_ref[15];
	guint8		message_type;
	guint8		info_element;
	guint16		info_element_len;
	int		codeset, locked_codeset;
	gboolean	non_locking_shift;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "Q.933");

	if (tree) {
		ti = proto_tree_add_item(tree, proto_q933, tvb, offset, -1,
		    ENC_NA);
		q933_tree = proto_item_add_subtree(ti, ett_q933);

		dissect_q933_protocol_discriminator(tvb, offset, q933_tree);
	}
	offset += 1;
	call_ref_len = tvb_get_guint8(tvb, offset) & 0xF;	/
	if (q933_tree != NULL)
		proto_tree_add_uint(q933_tree, hf_q933_call_ref_len, tvb, offset, 1, call_ref_len);
	offset += 1;
	if (call_ref_len != 0) {
		tvb_memcpy(tvb, call_ref, offset, call_ref_len);
		proto_tree_add_boolean(q933_tree, hf_q933_call_ref_flag,
			tvb, offset, 1, (call_ref[0] & 0x80) != 0);
		call_ref[0] &= 0x7F;
		proto_tree_add_bytes(q933_tree, hf_q933_call_ref,
			tvb, offset, call_ref_len, call_ref);
		offset += call_ref_len;
	}
	message_type = tvb_get_guint8(tvb, offset);
	col_add_str(pinfo->cinfo, COL_INFO,
		    val_to_str(message_type, q933_message_type_vals,
		      "Unknown message type (0x%02X)"));

	proto_tree_add_uint(q933_tree, hf_q933_message_type, tvb, offset, 1, message_type);
	offset += 1;

	/
	codeset = locked_codeset = 0;	/
	while (tvb_reported_length_remaining(tvb, offset) > 0) {
		info_element = tvb_get_guint8(tvb, offset);

		 /
		if ((info_element & Q933_IE_SO_MASK) &&
		    ((info_element & Q933_IE_SO_IDENTIFIER_MASK) == Q933_IE_SHIFT)) {
			non_locking_shift = info_element & Q933_IE_SHIFT_NON_LOCKING;
			codeset = info_element & Q933_IE_SHIFT_CODESET;
			if (!non_locking_shift)
				locked_codeset = codeset;
			proto_tree_add_item(q933_tree, non_locking_shift ? hf_q933_non_locking_shift_to_codeset : hf_q933_locking_shift_to_codeset, tvb, offset, 1, ENC_NA);
			offset += 1;
			continue;
		}

		/
		if (info_element & Q933_IE_SO_MASK) {
			switch ((codeset << 8) | (info_element & Q933_IE_SO_IDENTIFIER_MASK)) {

			case CS0 | Q933_IE_REPEAT_INDICATOR:
				proto_tree_add_item(q933_tree, hf_q933_repeat_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
				break;

			default:
				proto_tree_add_expert_format(q933_tree, pinfo, &ei_q933_information_element, tvb, offset, 1,
						"Unknown information element (0x%02X)", info_element);
				break;
			}
			offset += 1;
			codeset = locked_codeset;
			continue;
		}

		/
		info_element_len = tvb_get_guint8(tvb, offset + 1);
		if (q933_tree != NULL) {
			ie_tree = proto_tree_add_subtree(q933_tree, tvb, offset,
			    1+1+info_element_len, ett_q933_ie, NULL,
			    val_to_str(info_element, q933_info_element_vals[codeset],
			      "Unknown information element (0x%02X)"));
			proto_tree_add_uint_format_value(ie_tree, hf_q933_information_element, tvb, offset, 1, info_element,
								"%s", val_to_str(info_element, q933_info_element_vals[codeset], "Unknown (0x%02X)"));
			proto_tree_add_item(ie_tree, hf_q933_length, tvb, offset + 1, 1, ENC_BIG_ENDIAN);

			switch ((codeset << 8) | info_element) {

			case CS0 | Q933_IE_SEGMENTED_MESSAGE:
				dissect_q933_segmented_message_ie(tvb,
				    pinfo, offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_BEARER_CAPABILITY:
			case CS0 | Q933_IE_LOW_LAYER_COMPAT:
				dissect_q933_bearer_capability_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_CAUSE:
				dissect_q933_cause_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree,
				    hf_q933_cause_value);
				break;

			case CS0 | Q933_IE_CALL_STATE:
				dissect_q933_call_state_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_CHANNEL_IDENTIFICATION:
				dissect_q933_channel_identification_ie(
				    tvb, offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_PROGRESS_INDICATOR:
				dissect_q933_progress_indicator_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_NETWORK_SPECIFIC_FACIL:
			case CS0 | Q933_IE_TRANSIT_NETWORK_SEL:
				dissect_q933_ns_facilities_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_DISPLAY:
				dissect_q933_ia5_ie(tvb, offset + 2,
				    info_element_len, ie_tree,
				    hf_q933_display_information);
				break;

			case CS0 | Q933_IE_E2E_TRANSIT_DELAY:
				dissect_q933_e2e_transit_delay_ie(tvb,
				    pinfo, offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_TD_SELECTION_AND_INT:
				dissect_q933_td_selection_and_int_ie(
				    tvb, pinfo, offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_PL_BINARY_PARAMETERS:
				dissect_q933_pl_binary_parameters_ie(
				    tvb, offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_REVERSE_CHARGE_IND:
				dissect_q933_reverse_charge_ind_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_CALLING_PARTY_NUMBER:
				dissect_q933_number_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree,
				    hf_q933_calling_party_number);
				break;

			case CS0 | Q933_IE_CONNECTED_NUMBER:
				dissect_q933_number_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree,
				    hf_q933_connected_number);
				break;

			case CS0 | Q933_IE_CALLED_PARTY_NUMBER:
				dissect_q933_number_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree,
				    hf_q933_called_party_number);
				break;

			case CS0 | Q933_IE_CALLING_PARTY_SUBADDR:
			case CS0 | Q933_IE_CALLED_PARTY_SUBADDR:
				dissect_q933_party_subaddr_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_HIGH_LAYER_COMPAT:
				dissect_q933_high_layer_compat_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS0 | Q933_IE_USER_USER:
				dissect_q933_user_user_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;


			case CS0 | Q933_IE_REPORT_TYPE:
			case CS5 | Q933_IE_REPORT_TYPE:
			case CS5 | Q933_IE_ANSI_REPORT_TYPE:
				dissect_q933_report_type_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS5 | Q933_IE_LINK_INTEGRITY_VERF:
			case CS5 | Q933_IE_ANSI_LINK_INTEGRITY_VERF:
				dissect_q933_link_integrity_verf_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			case CS5 | Q933_IE_PVC_STATUS:
			case CS5 | Q933_IE_ANSI_PVC_STATUS:
				dissect_q933_pvc_status_ie(tvb,
				    offset + 2, info_element_len,
				    ie_tree);
				break;

			default:
				proto_tree_add_item(ie_tree, hf_q933_data, tvb, offset + 2, info_element_len, ENC_NA);
				break;
			}
		}
		offset += 1 + 1 + info_element_len;
		codeset = locked_codeset;
	}
	return tvb_captured_length(tvb);
}
