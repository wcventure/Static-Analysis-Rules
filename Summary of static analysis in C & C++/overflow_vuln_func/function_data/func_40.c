static void
decode_ip_device_routing(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
			 guint offset, guint length, guint8 opcode _U_,
			 proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	gint *lengthp = 0;
	proto_tree *ua3g_body_tree;
	proto_item *ua3g_param_item;
	proto_tree *ua3g_param_tree;
	static const value_string str_command[] = {
		{0x00, "Reset"},
		{0x01, "Start RTP"},
		{0x02, "Stop RTP"},
		{0x03, "Redirect"},
		{0x04, "Tone Definition"},
		{0x05, "Start Tone"},
		{0x06, "Stop Tone"},
		{0x07, "Start Listen RTP"},
		{0x08, "Stop Listen RTP"},
		{0x09, "Get Parameters Value"},
		{0x0A, "Set Parameters Value"},
		{0x0B, "Send Digit"},
		{0x0C, "Pause RTP"},
		{0x0D, "Restart RTP"},
		{0x0E, "Start Record RTP"},
		{0x0F, "Stop Record RTP"},
		{0, NULL}
	};

	emem_strbuf_t *strbuf = ep_strbuf_new_label("");

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

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_ip, tvb, offset, 1,
		command, "Command: %s",
		val_to_str(command, str_command, "Unknown"));
	offset++;
	length--;

	switch(command) {
	case 0x00: /
		{
			int i, parameter_id, parameter_length;
			static const value_string str_parameter_id[] = {
				{0x00, "Update Mode"},
				{0x01, "Bad Sec Mode"},
				{0x02, "Customization Name"},
				{0x03, "Localization Name"},
				{0, NULL}
			};

			if(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x00: /
						{
							static const value_string str_update_mode[] = {
								{0x00, "NOE Update Mode"},
								{0x01, "Bootloader"},
								{0x02, "Data"},
								{0x04, "Customization"},
								{0x08, "Localization"},
								{0x10, "Code"},
								{0x20, "SIP"},
								{0, NULL}
							};
							static const value_string str_disable_enable[] = {
								{0, "Disable"},
								{1, "Enable"},
								{0, NULL}
							};

							if((tvb_get_guint8(tvb, offset + 2) & 0x80) == 0x00) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s: ",
									val_to_str(tvb_get_guint8(tvb, offset + 2) & 0x80, str_update_mode, "Unknown"));

								for(i = 0; i < 6; i++) {
									ep_strbuf_append_printf(strbuf, "%s: %s, ",
										val_to_str(0x01 << i, str_update_mode, "Unknown"),
										val_to_str((tvb_get_guint8(tvb, offset + 2) >> i) & 0x01, str_disable_enable, ""));
								}
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s",
									val_to_str(tvb_get_guint8(tvb, offset + 2) & 0x80, str_update_mode, "Unknown"));
							}

							break;
						}
					case 0x01: /
						{
							static const value_string str_bad_sec_mode[] = {
								{0x01, "Binary is full, CS is secured, but terminal running in clear mode"},
								{0, NULL}
							};

							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf,
								val_to_str(tvb_get_guint8(tvb, offset + 2), str_bad_sec_mode, "Unknown"));
							break;
						}
					case 0x02: /
						{
							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, "\"");
							for(i = 1; i <= parameter_length; i++) {
								if(isprint(tvb_get_guint8(tvb, offset + 1 + i)))
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
								else
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
							}

							ep_strbuf_append(strbuf, "\"");
							break;
						}
					case 0x03: /
						{
							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, "\"");
							for(i = 1; i <= parameter_length; i++) {
								if(isprint(tvb_get_guint8(tvb, offset + 1 + i)))
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
								else
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
							}

							ep_strbuf_append(strbuf, "\"");
							break;
						}
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s", val_to_str(parameter_id, str_parameter_id, "Unknown"));
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					/
					/
					/
				}
			}
			break;
		}
	case 0x01: /
		{
			int i, parameter_length, parameter_id;
			static const value_string str_direction[] = {
				{0x00, "Terminal Input"},
				{0x01, "Terminal Output"},
				{0x02, "Terminal Input/Output (Both Directions)"},
				{0, NULL}
			};
			static const value_string str_parameter_id[] = {
				{0x00, "Local UDP Port"},
				{0x01, "Remote IP Address"},
				{0x02, "Remote UDP Port"},
				{0x03, "Type Of Service"},
				{0x04, "Compressor"},
				{0x05, "Payload Concatenation (ms)"},
				{0x06, "Echo Cancelation Enabler"},
				{0x07, "Silence Suppression Enabler"},
				{0x08, "802.1 Q User Priority"},
				{0x09, "Reserved"},
				{0x0a, "Post Filtering Enabler"},
				{0x0b, "High Pass Filtering Enabler"},
				{0x0c, "Remote SSRC"},
				{0x0d, "Must Send QOS Tickets"},
				{0x0e, "Local Identifier"},
				{0x0f, "Distant Identifier"},
				{0x10, "Destination For RTCP Sender Reports - Port Number"},
				{0x11, "Destination For RTCP Sender Reports - IP Address"},
				{0x12, "Destination For RTCP Receiver Reports - Port Number"},
				{0x13, "Destination For RTCP Receiver Reports - IP Address"},
				{0x14, "Channel Number"},
				{0x15, "DTMF Sending"},
				{0x16, "Payload Type Of Redundancy"},
				{0x17, "Payload Type Of DTMF Events"},
				{0x18, "Enable / Disable RFC 2198"},
				{0x31, "SRTP Encryption Enable For This Communication"},
				{0x32, "Master Key For SRTP Session"},
				{0x33, "Master Salt Key For SRTP Session"},
				{0x34, "Master key for output stream of SRTP session"},
				{0x35, "Master salt key for output stream of SRTP session"},
				{0x36, "Integrity checking enabled for this communication"},
				{0x37, "MKI value for SRTP packets in input stream"},
				{0x38, "MKI value for SRTP packets in output stream"},
				{0x50, "MD5 Authentication"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Direction: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_direction, "Unknown"));
			offset++;
			length--;

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x01: /
					case 0x11: /
					case 0x13: /
						{
							if(parameter_length > 0) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 2));
							}

							for(i = 2; i <= parameter_length; i++) {
								ep_strbuf_append(strbuf, ".");
								ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+1+i));
							}
							break;
						}
					case 0x04: /
						{
							static const value_string str_compressor[] = {
								{0x00, "G.711 A-law"},
								{0x01, "G.711 mu-law"},
								{0x0F, "G.723.1 5.3kbps"},
								{0x10, "G.723.1 6.3kbps"},
								{0x11, "G.729A 8 kbps"},
								{0, NULL}
							};

							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;
								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_compressor, "Default Codec"));
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x06: /
					case 0x07: /
					case 0x0A: /
					case 0x0B: /
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_on_off, "On"));
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x0D: /
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_yes_no, "No"));
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x0E: /
					case 0x0F: /
						{
							break;
						}
					case 0x15: /
						{
							static const value_string str_dtmf_sending[] = {
								{0x00, "Don't Send DTMF"},
								{0, NULL}
							};
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--)
								{
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_dtmf_sending, "Send DTMF"));
							}
							else
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x18: /
						{
							static const value_string str_enable_00[] = {
								{0x00, "Enable"},
								{0, NULL}
							};

							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_enable_00, "Disable"));
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x31: /
						{
							static const value_string str_enable_10[] = {
								{0x10, "Enable"},
								{0, NULL}
							};

							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--)
								{
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, val_to_str((guint8)(param_value), str_enable_10, "Disable"));
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					case 0x00: /
					case 0x02: /
					case 0x03: /
					case 0x05: /
					case 0x08: /
					case 0x09: /
					case 0x0C: /
					case 0x10: /
					case 0x12: /
					case 0x14: /
					case 0x16: /
					case 0x17: /
					case 0x32: /
					case 0x33: /
					case 0x34: /
					case 0x35: /
					case 0x36: /
					case 0x37: /
					case 0x38: /
					case 0x50: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--)
								{
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x02: /
		{
			int i, parameter_id, parameter_length;
			static const value_string str_parameter_id[] = {
				{0x0E, "Local Identifier"},
				{0x0F, "Distant Identifier"},
				{0x14, "Canal Identifier"},
				{0, NULL}
			};

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x0E: /
					case 0x0F: /
						{
							break;
						}
					case 0x14: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--)
								{
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x03: /
		{
			int i, parameter_length, parameter_id;
			static const value_string str_parameter_id[] = {
				{0x00, "Remote MainCPU Server IP Adress"},
				{0x01, "Remote MainCPU Server Port"},
				{0, NULL}
			};

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x00: /
						{
							if(parameter_length > 0) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 2));
							}

							for(i = 2; i <= parameter_length; i++) {
								ep_strbuf_append(strbuf, ".");
								ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+1+i));
							}
							break;
						}
					case 0x01: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
						}
						break;
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset, parameter_length + 2,
						"%s: %s", val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset, parameter_length + 2,
						"%s", val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x04: /
		{
			int i, tone_nb_entries;
			guint16 frequency_1, frequency_2;
			signed char level_1, level_2;

			tone_nb_entries = tvb_get_guint8(tvb, offset);

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Number Of Entries: %d", tone_nb_entries);
			offset++;
			length--;

			while(length > 0 && tone_nb_entries) {
				for(i = 1; i <= tone_nb_entries; i++) {
					frequency_1 = tvb_get_ntohs(tvb, offset);
					level_1 = (signed char)(tvb_get_guint8(tvb, offset + 2)) / 2;
					frequency_2 = tvb_get_ntohs(tvb, offset + 3);
					level_2 = (signed char)(tvb_get_guint8(tvb, offset + 5)) / 2;

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset, 6,
						"Tone Pair %d: %d Hz at %d dB / %d Hz at %d dB",
						i, frequency_1, level_1, frequency_2, level_2);
					ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 2,
						"Frequency 1: %d Hz", frequency_1);
					offset += 2;
					length -= 2;

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Level: %d dB", level_1);
					offset++;
					length--;

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 2, "Frequency 2: %d Hz", frequency_2);
					offset += 2;
					length -= 2;

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Level: %d dB", level_2);
					offset++;
					length--;
				}
			}
			break;
		}
	case 0x05: /
		{
			guint8 i, tone_nb_entries, tone_direction, tone_id;
/
			int tone_duration;
			static const value_string str_tone_direction[] = {
				{0x00, "On The Phone"},
				{0x40, "To The Network"},
				{0x80, "On The Phone and To The Network"},
				{0, NULL}
			};

			tone_direction = tvb_get_guint8(tvb, offset) & 0xC0;
			tone_nb_entries = tvb_get_guint8(tvb, offset);

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Direction: %s - Number Of Entries: %d",
				val_to_str(tone_direction, str_tone_direction, "Unknown"), tone_nb_entries);
			offset++;
			length--;

			while(length > 0 && tone_nb_entries) {
				for(i = 1; i <= tone_nb_entries; i++) {
					tone_id = tvb_get_guint8(tvb, offset);
					tone_duration = tvb_get_ntohs(tvb, offset + 1);
					/

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset, 6,
						"Tone Pair %d: Id: %d, Duration: %d ms",
/
						i, tone_id, tone_duration);
/
					ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
						"Identification: %d", tone_id);
					offset++;
					length--;

					proto_tree_add_text(ua3g_param_tree, tvb, offset, 2,
						"Duration: %d ms", tone_duration);
					offset += 2;
					length -= 2;

					/
				}
			}
			break;
		}
	case 0x07: /
	case 0x08: /
		{
			int i, parameter_length, parameter_id;
			static const value_string str_parameter_id[] = {
				{0x00, "Remote IP Adress     "},
				{0x01, "Remote UDP Port In   "},
				{0x02, "Remote UDP Port Out  "},
				{0x03, "Remote IP Address Out"},
				{0x04, "Canal Number"},
				{0, NULL}
			};

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x00: /
					case 0x03: /
						{
							if(parameter_length > 0) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 2));
							}

							for(i = 2; i <= parameter_length; i++) {
								ep_strbuf_append(strbuf, ".");
								ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+1+i));
							}
							break;
						}
					case 0x01: /
					case 0x02: /
					case 0x04: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
						}
						break;
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				}
				else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x09: /
		{
			guint8 parameter;
			static const value_string str_parameter[] = {
				{0x00	, "Firmware Version"},
				{0x01	, "Firmware Version"},
				{0x02	, "DHCP IP Address"},
				{0x03	, "Local IP Address"},
				{0x04	, "Subnetwork Mask"},
				{0x05	, "Router IP Address"},
				{0x06	, "TFTP IP Address"},
				{0x07	, "MainCPU IP Address"},
				{0x08	, "Default Codec"},
				{0x09	, "Ethernet Drivers Config"},
				{0x0A	, "MAC Address"},
				{0, NULL}
			};

			while(length > 0) {
				parameter = tvb_get_guint8(tvb, offset);
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s",
					val_to_str(parameter, str_parameter, "Unknown"));
				offset++;
				length--;
			}
			break;
		}
	case 0x0A: /
		{
			int i, parameter_id, parameter_length;
			static const value_string str_parameter_id[] = {
				{0x00	, "QOS IP TOS"},
				{0x01	, "QOS 8021 VLID"},
				{0x02	, "QOS 8021 PRI"},
				{0x03	, "SNMP MIB2 SysContact"},
				{0x04	, "SNMP MIB2 SysName"},
				{0x05	, "SNMP MIB2 SysLocation"},
				{0x06	, "Default Compressor"},
				{0x07	, "Error String Net Down"},
				{0x08	, "Error String Cable PB"},
				{0x09	, "Error String Try Connect"},
				{0x0A	, "Error String Connected"},
				{0x0B	, "Error String Reset"},
				{0x0C	, "Error String Duplicate IP Address"},
				{0x0D	, "SNMP MIB Community"},
				{0x0E	, "TFTP Backup Sec Mode"},
				{0x0F	, "TFTP Backup IP Address"},
				{0x10	, "Set MMI Password"},
				{0x11	, "Set PC Port Status"},
				{0x12	, "Record RTP Authorization"},
				{0x13	, "Security Flags"},
				{0x14	, "ARP Spoofing"},
				{0x15	, "Session Param"},
				{0x30	, "MD5 Authentication"},
				{0, NULL}
			};

			while(length > 0) {
				guint64 param_value = 0;

				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x06: /
						{
							static const value_string str_compressor[] = {
								{0x00, "G.711 A-law"},
								{0x01, "G.711 mu-law"},
								{0x0F, "G.723.1 5.3kbps"},
								{0x10, "G.723.1 6.3kbps"},
								{0x11, "G.729A 8 kbps"},
								{0, NULL}
							};

							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, val_to_str(tvb_get_guint8(tvb, offset + 2),
								str_compressor, "Default Codec"));
							break;
						}
					case 0x07: /
					case 0x08: /
					case 0x09: /
					case 0x0A: /
					case 0x0B: /
					case 0x0C: /
						{
							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, "\"");
							for(i = 1; i <= parameter_length; i++) {
								if(isprint(tvb_get_guint8(tvb, offset + 1 + i)))
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
								else
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
							}
							ep_strbuf_append(strbuf, "\"");
							break;
						}
					case 0x0F: /
						{
							if(parameter_length > 0) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 2));
							}

							for(i = 2; i <= parameter_length; i++) {
								ep_strbuf_append(strbuf, ".");
								ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+1+i));
							}
							break;
						}
					case 0x11: /
						{
							static const value_string str_set_pc_port_status[] = {
								{0x00, "No PC Port Security"},
								{0x01, "Block PC Port"},
								{0x02, "Filter VLAN"},
								{0, NULL}
							};
							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, val_to_str(tvb_get_guint8(tvb, offset + 2),
								str_set_pc_port_status, "Unknown"));
							break;
						}
					case 0x12: /
						{
							static const value_string str_enable_feature[] = {
								{0x00, "Disable Feature"},
								{0x01, "Enable Feature"},
								{0, NULL}
							};

							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, val_to_str(tvb_get_guint8(tvb, offset + 2),
								str_enable_feature, "Unknown"));
							break;
						}
					case 0x13: /
						{
							static const value_string str_set_pc_port_status[] = {
								{0x00, "Filtering Not Active"},
								{0x01, "Filtering Activated"},
								{0, NULL}
							};
							ep_strbuf_truncate(strbuf, 0);
							ep_strbuf_append(strbuf, val_to_str((tvb_get_guint8(tvb, offset + 2) & 0x01),
								str_set_pc_port_status, "Unknown"));
							break;
						}
					case 0x00: /
					case 0x01: /
					case 0x02: /
					case 0x03: /
					case 0x04: /
					case 0x05: /
					case 0x0D: /
					case 0x0E: /
					case 0x10: /
					case 0x14: /
					case 0x15: /
					case 0x30: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else if(parameter_length > 8) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
							break;
						}
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x0B: /
		{
			static const value_string str_digit[] = {
				{0, "0"},
				{1, "1"},
				{2, "2"},
				{3, "3"},
				{4, "4"},
				{5, "5"},
				{6, "6"},
				{7, "7"},
				{8, "8"},
				{9, "9"},
				{10, "*"},
				{11, "#"},
				{12, "A"},
				{13, "B"},
				{14, "C"},
				{15, "D"},
				{16, "Flash"},
				{0, NULL}
			};
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Digit Value: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_digit, "Unknown"));
		break;
		}
	case 0x0C: /
	case 0x0D: /
		{
			int i, parameter_length, parameter_id;
			static const value_string str_parameter_id[] = {
				{0x14, "Canal Identifier"},
				{0, NULL}
			};

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					if((parameter_length > 0) && (parameter_length <= 8)) {
						guint64 param_value = 0;

						for(i = parameter_length; i > 0; i--) {
							param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
						}
						ep_strbuf_truncate(strbuf, 0);
						ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
					} else {
						ep_strbuf_truncate(strbuf, 0);
						ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
							tvb_get_guint8(tvb, offset + 2),
							tvb_get_guint8(tvb, offset + 3),
							tvb_get_guint8(tvb, offset + parameter_length),
							tvb_get_guint8(tvb, offset + 1 + parameter_length));
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				}
				else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x0E: /
	case 0x0F: /
		{
			int i, parameter_length, parameter_id;
			static const value_string str_parameter_id[] = {
				{0x00	, "Recorder Index"},
				{0x01	, "Remote IP Address"},
				{0x02	, "Remote UDP Port In"},
				{0x03	, "Remote UDP Port Out"},
				{0x04	, "Remote IP Address Out"},
				{0x05	, "Local UDP Port In"},
				{0x06	, "Local UDP Port Out"},
				{0x07	, "Type Of Service"},
				{0x08	, "Master Key For SRTP Session"},
				{0x09	, "Master Salt Key For SRTP Session"},
				{0x30	, "MD5 Authentication"},
				{0, NULL}
			};

			while(length > 0) {
				parameter_id = tvb_get_guint8(tvb, offset);
				parameter_length = tvb_get_guint8(tvb, offset + 1);

				if(parameter_length > 0) {
					switch(parameter_id) {
					case 0x01: /
					case 0x04: /
						{
							if(parameter_length > 0) {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 2));
							}

							for(i = 2; i <= parameter_length; i++) {
								ep_strbuf_append(strbuf, ".");
								ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+1+i));
							}
							break;
						}
					case 0x00: /
					case 0x02: /
					case 0x03: /
					case 0x05: /
					case 0x06: /
					case 0x07: /
					case 0x08: /
					case 0x09: /
					case 0x30: /
					default:
						{
							if((parameter_length > 0) && (parameter_length <= 8)) {
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 1 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
							} else {
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "0x%02x 0x%02x ... 0x%02x 0x%02x",
									tvb_get_guint8(tvb, offset + 2),
									tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + parameter_length),
									tvb_get_guint8(tvb, offset + 1 + parameter_length));
							}
						}
						break;
					}

					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s: %s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
				} else
					ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
						parameter_length + 2, "%s",
						val_to_str(parameter_id, str_parameter_id, "Unknown"));

				ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);
				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Parameter: %s (0x%02x)",
					val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
				offset++;
				length--;

				proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
					"Length: %d", parameter_length);
				offset++;
				length--;

				if(parameter_length > 0) {
					proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
						"Value: %s", strbuf->str);
					offset += parameter_length;
					length -= parameter_length;
				}
			}
			break;
		}
	case 0x06: /
	default:
		{
			break;
		}
	}
}
