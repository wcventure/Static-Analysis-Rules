static void
decode_audio_config(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		    guint offset, guint length, guint8 opcode _U_,
		    proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_command[] = {
		{0x00, "Audio Coding"},
		{0x01, "DPI Channel Allocations"},
		{0x02, "Loudspeaker Volume Adjust"},
		{0x03, "Audio Circuit Configuration"},
		{0x04, "Handsfree Parameters"},
		{0x05, "Loudspeaker Acoustic Parameters"},
		{0x06, "Device Congiguration"},
		{0, NULL}
	};

	/
	proto_item_append_text(ua3g_item, ", %s",
		val_to_str(command, str_command, "Unknown"));
	proto_item_append_text(ua3g_body_item, " - %s",
		val_to_str(command, str_command, "Unknown"));
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(command, str_command, "Unknown"));

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_command, tvb,
		offset, 1, command, "Command: %s (%d)",
		val_to_str(command, str_command, "Unknown"), command);
	offset++;
	length--;

	switch(command) {
	case 0x00: /
		{
			static const value_string str_law[] = {
				{0x00, "A Law"},
				{0x01, "m Law"},
				{0, NULL}
			};
			proto_tree_add_text(ua3g_body_tree, tvb, offset,
				1, "Ignored: %d",
				tvb_get_guint8(tvb, offset));
			offset++;
			length--;

			proto_tree_add_text(ua3g_body_tree, tvb, offset,
				1, "Law: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_law, "Unknown"));
			break;
		}
	case 0x01: /
		{
			int i;
			static const value_string str_body[] = {
				{0, "UA Channel UA-TX1   "},
				{1, "UA Channel UA-TX2   "},
				{2, "GCI Channel GCI-TX1 "},
				{3, "GCI Channel GCI-TX2 "},
				{4, "Codec Channel COD-TX"},
				{0, NULL}
			};
			for(i = 0; i < 5; i++) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset,
					1, "%s: %d",
					val_to_str(i, str_body, "Unknown"), tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}
			break;
		}
	case 0x02: /
		{
			int i;
			for(i = 1; i < 8; i++) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset,
					1, "Volume Level %d: %d",
					i, tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}
			break;
		}
	case 0x03: /
		{
			int i;
			static const value_string str_body[] = {
				{0, "Anti-Distortion Coeff 1(DTH)"},
				{1, "Anti-Distortion Coeff 2(DTR)"},
				{2, "Anti-Distortion Coeff 3(DTF)"},
				{3, "Sidetone Attenuation (STR)  "},
				{4, "Anti-Larsen Coeff 1 (AHP1)  "},
				{5, "Anti-Larsen Coeff 2 (AHP2)  "},
				{6, "Anti-Larsen Coeff 3 (ATH)   "},
				{7, "Anti-Larsen Coeff 4 (ATR)   "},
				{8, "Anti-Larsen Coeff 5 (ATF)   "},
				{9, "Anti-Larsen Coeff 6 (ALM)   "},
				{0, NULL}
			};

			for(i = 0; i < 10; i++) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset,
					1, "%s: %d",
					val_to_str(i, str_body, "Unknown"), tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}
			break;
		}
	case 0x04: /
		{
			static const value_string str_return_loss[] = {
				{0x00, "Return Loss Normal"},
				{0x01, "Return Loss Active"},
				{0, NULL}
			};
			static const value_string str_full_duplex[] = {
				{0x00, "Handsfree Normal"},
				{0x02, "More Full Duplex"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
				val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_return_loss, "Unknown"));
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
				val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_full_duplex, "Unknown"));
			break;
		}
	case 0x05: /
		{
			int i;
			static const value_string str_body[] = {
				{0, "Group Listening Attenuation Constant                                      "},
				{1, "Handsfree Attenuation Constant                                            "},
				{2, "Handsfree Number Of ms To Stay In Send State Before Going To Another State"},
				{3, "Handsfree Number Of Positions To Shift Right MTx                          "},
				{4, "Handsfree Number Of Positions To Shift Right MRc                          "},
				{5, "Handsfree Idle Transmission Threshold                                     "},
				{6, "Handsfree Low Transmission Threshold                                      "},
				{7, "Handsfree Idle Reception Threshold                                        "},
				{8, "Handsfree Low Reception Threshold                                         "},
				{9, "Handsfree Medium Reception Threshold                                      "},
				{10, "Handsfree High Reception Threshold                                        "},
				{0, NULL}
			};

			for(i = 0; i < 11; i++) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s: %d",
					val_to_str(i, str_body, "Unknown"), tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}
			break;
		}
	case 0x06: /
		{
			static const value_string str_device[] = {
				{0, "Handset Device             "},
				{1, "Headset Device             "},
				{2, "Loudspeaker Device         "},
				{3, "Announce Loudspeaker Device"},
				{4, "Handsfree Device           "},
				{0, NULL}
			};
			static const value_string str_device_values[] = {
				{0x01, " Internal"},
				{0x02, " Rj9 Plug"},
				{0x04, " Jack Plug"},
				{0x08, " Bluetooth Link"},
				{0x10, " USB Link"},
				{0, NULL}
			};
			emem_strbuf_t *strbuf = ep_strbuf_new_label("");
			int j;
			int i = 0;

			while(length > 0) {
				ep_strbuf_truncate(strbuf, 0);
				ep_strbuf_append_printf(strbuf, "%s",
					val_to_str(tvb_get_guint8(tvb, offset) & 0x01, str_device_values, ""));

				for(j = 1; j < 4; j++) {
					ep_strbuf_append_printf(strbuf, "%s",
						val_to_str((tvb_get_guint8(tvb, offset) & (0x01 << j)), str_device_values, ""));
				}
				if(strcmp(strbuf->str, "") <= 0) {
					ep_strbuf_truncate(strbuf, 0);
					ep_strbuf_append(strbuf, " None");
				}

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s:%s",
					val_to_str(i, str_device, "Unknown"), strbuf->str);
				offset++;
				length--;
				i++;
			}
			break;
		}
	default:
		{
			break;
		}
	}
}
