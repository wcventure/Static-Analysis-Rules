static void
decode_cs_ip_device_routing(proto_tree *tree _U_, tvbuff_t *tvb,
			    packet_info *pinfo, guint offset, guint length,
			    guint8 opcode _U_, proto_item *ua3g_item,
			    proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	gint *lengthp = 0;
	proto_tree *ua3g_body_tree;
	proto_item *ua3g_param_item;
	proto_tree *ua3g_param_tree;
	emem_strbuf_t *strbuf = ep_strbuf_new_label("");
	static const value_string str_command[] = {
		{0x00, "Init"},
		{0x01, "Incident"},
		{0x02, "Get Parameters Value Response"},
		{0x03, "QOS Ticket RSP"},
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

	proto_tree_add_uint_format(ua3g_body_tree, hf_ua3g_ip, tvb, offset,
		1, command, "Command: %s",
		val_to_str(command, str_command, "Unknown"));
	offset++;
	length--;

	switch(command) {
		case 0x00:
			{
				static const value_string str_vta_type[] = {
					{0x20, "NOE A"},
					{0x21, "NOE B"},
					{0x22, "NOE C"},
					{0x23, "NOE D"},
					{0, NULL}
				};
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "VTA Type: %s",
					val_to_str(tvb_get_guint8(tvb, offset), str_vta_type, "Unknown"));
				offset++;
				length--;

				proto_tree_add_text(ua3g_body_tree, tvb, offset,
					1, "Characteristic Number: %d", tvb_get_guint8(tvb, offset));
				break;
			}
		case 0x01:
			{
				int i=0;
				if(length == 1) {
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
						"Incident 0: %d", tvb_get_guint8(tvb, offset));
				} else {
					while(length >0) {
						i++;
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
							"Parameter %d Identifier: %d",
							i, tvb_get_guint8(tvb, offset));
						offset++;
						length--;
					}
				}
				break;
			}
		case 0x02:
			{
				int i, parameter_id, parameter_length;
				static const value_string str_parameter_id[] = {
					{0x00, "Firmware Version"},
					{0x01, "Firmware Version"},
					{0x02, "DHCP IP Address"},
					{0x03, "Local IP Address"},
					{0x04, "Subnetwork Mask"},
					{0x05, "Router IP Address"},
					{0x06, "TFTP IP Address"},
					{0x07, "Main CPU Address"},
					{0x08, "Default Codec"},
					{0x09, "Ethernet Drivers Config"},
					{0x0A, "MAC Address"},
					{0, NULL}
				};

				while(length > 0) {
					parameter_id = tvb_get_guint8(tvb, offset);
					parameter_length = tvb_get_guint8(tvb, offset + 1);

					if(parameter_length > 0) {
						switch(parameter_id) {
						case 0x00: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s",
									version_number_computer(tvb_get_ntohs(tvb, offset + 2)));
								break;
							}
						case 0x01: /
						case 0x02: /
						case 0x03: /
						case 0x04: /
						case 0x05: /
						case 0x06: /
						case 0x07: /
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
						case 0x08: /
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
									ep_strbuf_append(strbuf,
										val_to_str((guint8)(param_value), str_compressor, "Default Codec"));
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
						case 0x09: /
							{
								if(parameter_length == 2) {
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append_printf(strbuf,
										"Port Lan Speed: %d - Port Lan Duplex: %d",
										tvb_get_guint8(tvb, offset + 2),
										tvb_get_guint8(tvb, offset + 3));
								} else if(parameter_length == 4) {
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append_printf(strbuf,
										"Port Lan Speed: %d - Port Lan Duplex: %d - Port PC Speed: %d - Port PC Duplex: %d",
										tvb_get_guint8(tvb, offset + 2),
										tvb_get_guint8(tvb, offset + 3),
										tvb_get_guint8(tvb, offset + 4),
										tvb_get_guint8(tvb, offset + 5));
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
						case 0x0A: /
							{
								if(parameter_length > 0) {
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append_printf(strbuf, "%02x", tvb_get_guint8(tvb, offset + 2));
								}

								for(i = 2; i <= parameter_length; i++) {
									ep_strbuf_append(strbuf, ":");
									ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+1+i, lengthp));
								}
								break;
							}
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
					proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Parameter: %s (0x%02x)",
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
		case 0x03:
			{
				int i, parameter_id, parameter_length;
				int element_length = 1;
				int framing_rtp = 0;
				char parameter_value_tab[15][50];
				static const value_string str_parameter_id[] = {
					{0x01, "Date Of End Of Communication"},
					{0x02, "Node Number"},
					{0x03, "Ticket Protocol Version"},
					{0x06, "Equiment Type"},
					{0x08, "Local IP Address"},
					{0x09, "Distant IP Address"},
					{0x0A, "Local ID"},
					{0x0B, "Distant ID"},
					{0x0C, "Call Duration (second)"},
					{0x0D, "Local SSRC"},
					{0x0E, "Distant SSRC"},
					{0x0F, "Codec"},
					{0x10, "VAD"},
					{0x11, "ECE"},
					{0x12, "Voice Mode"},
					{0x13, "Transmitted Framing (ms)"},
					{0x14, "Received Framing (ms)"},
					{0x15, "Framing Changes"},
					{0x16, "Number Of RTP Packets Received"},
					{0x17, "Number Of RTP Packets Sent"},
					{0x18, "Number Of RTP Packets Lost"},
					{0x19, "Total Silence Detected (second)"},
					{0x1A, "Number Of SID Received"},
					{0x1B, "Delay Distribution"},
					{0x1C, "Maximum Delay (ms)"},
					{0x1D, "Number Of DTMF Received"},
					{0x1E, "Consecutive BFI"},
					{0x1F, "BFI Distribution"},
					{0x20, "Jitter Depth Distribution"},
					{0x21, "Number Of ICMP Host Unreachable"},
					{0x26, "Firmware Version"},
					{0x29, "DSP Framing (ms)"},
					{0x2A, "Transmitter SID"},
					{0x2D, "Minimum Delay (ms)"},
					{0x2E, "802.1 Q Used"},
					{0x2F, "802.1p Priority"},
					{0x30, "VLAN Id"},
					{0x31, "DiffServ"},
					{0x3D, "200 ms BFI Distribution"},
					{0x3E, "Consecutive RTP Lost"},
					{0, NULL}
				};
				static const value_string str_parameter_id_tab[] = {
					{0x1B, "Range: Value"},
					{0x1F, "Range: Value"},
					{0x20, "Jitter: Value"},
					{0x3D, "Contents: Value"},
					{0x3E, "Contents: Value"},
					{0, NULL}
				};

				while(length > 0) {
					parameter_id = tvb_get_guint8(tvb, offset);
					parameter_length = tvb_get_ntohs(tvb, offset + 1);

					if(parameter_length > 0) {
						switch(parameter_id) {
						case 0x06: /
							{
								static const value_string str_first_byte[] = {
									{0x01, "IP-Phone"},
									{0x02, "Appli-PC"},
									{0x03, "Coupler OmniPCX Enterprise"},
									{0x04, "Coupler OmniPCX Office"},
									{0, NULL}
								};
								static const value_string str_second_byte[] = {
									{0x0101, "IP-Phone V2"},
									{0x0102, "NOE-IP"},
									{0x0200, "4980 Softphone (PCMM2)"},
									{0x0201, "WebSoftphoneIP"},
									{0x0300, "INTIP"},
									{0x0301, "GD"},
									{0x0302, "eVA"},
									{0, NULL}
								};
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s, %s",
									val_to_str(tvb_get_guint8(tvb, offset + 3), str_first_byte, "Unknown"),
									val_to_str(tvb_get_ntohs(tvb, offset + 3), str_second_byte, "Unknown"));
								break;
							}
						case 0x08: /
						case 0x09: /
						case 0x26: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", tvb_get_guint8(tvb, offset + 3));
								for(i = 2; i <= parameter_length; i++) {
									ep_strbuf_append(strbuf, ".");
									ep_strbuf_append_printf(strbuf, "%u", tvb_get_guint8(tvb, offset+2+i));
								}
								break;
							}
						case 0x0A:
						case 0x0B:
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append(strbuf, "\"");
								for(i = 1; i <= parameter_length; i++) {
									if(isprint(tvb_get_guint8(tvb, offset + 2 + i)))
										ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+2+i, lengthp));
									else
										ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+2+i, lengthp));
								}
								ep_strbuf_append(strbuf, "\"");
								break;
							}
						case 0x0F: /
							{
								static const value_string str_compressor[] = {
									{0x00, "G.711 A-law"},
									{0x01, "G.711 mu-law"},
									{0x02, "G.723.1 6.3kbps"},
									{0x03, "G.729"},
									{0x04, "G.723.1 5.3kbps"},
									{0, NULL}
								};

								if((parameter_length > 0) && (parameter_length <= 8)) {
									guint64 param_value = 0;

									for(i = parameter_length; i > 0; i--) {
										param_value += (tvb_get_guint8(tvb, offset + 2 + i) << (8 * (parameter_length - i)));
									}
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append(strbuf,
										val_to_str((guint8)(param_value), str_compressor, "Default Codec"));
								} else {
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append(strbuf, "Parameter Value Too Long (more than 64 bits)");
								}

								break;
							}
						case 0x10: /
						case 0x11: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s",
									val_to_str(tvb_get_guint8(tvb, offset + 3), str_on_off, "On"));
								break;
							}
						case 0x12: /
							{
								static const value_string str_voice_mode[] = {
									{0x50, "Idle"},
									{0x51, "Handset"},
									{0x52, "Group Listening"},
									{0x53, "On Hook Dial"},
									{0x54, "Handsfree"},
									{0x55, "Headset"},
									{0, NULL}
								};

								if((parameter_length > 0) && (parameter_length <= 8)) {
									guint64 param_value = 0;

									for(i = parameter_length; i > 0; i--) {
										param_value += (tvb_get_guint8(tvb, offset + 2 + i) << (8 * (parameter_length - i)));
									}
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append(strbuf,
										val_to_str((guint8)(param_value), str_voice_mode, "Unknown"));
								} else {
									ep_strbuf_truncate(strbuf, 0);
									ep_strbuf_append(strbuf, "Parameter Value Too Long (more than 64 bits)");
								}

								break;
							}
						case 0x1B: /
							{
								static const value_string str_range[] = {
									{0, "0-40     "},
									{1, "40-80    "},
									{2, "80-150   "},
									{3, "150-250  "},
									{4, "250 and +"},
									{0, NULL}
								};
								element_length = 2;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "%s: %d",
										val_to_str(i, str_range, "Unknown"),
										tvb_get_ntohs(tvb, offset + 3 + element_length * i));
								}
								break;
							}
						case 0x1E: /
							{
								static const value_string str_range[] = {
									{0, "0"},
									{1, "1"},
									{2, "2"},
									{3, "3"},
									{4, "4"},
									{5, "5"},
									{6, "5"},
									{7, "7"},
									{8, "8"},
									{9, "9"},
									{0, NULL}
								};
								element_length = 2;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "%s: %d",
										val_to_str(i, str_range, "Unknown"),
										tvb_get_ntohs(tvb, offset + 3 + element_length * i));
								}
								break;
							}
						case 0x1F: /
							{
								static const value_string str_range[] = {
									{0, "0      "},
									{1, "0-1    "},
									{2, "1-2    "},
									{3, "2-3    "},
									{4, "3 and +"},
									{0, NULL}
								};
								element_length = 2;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "%s: %d",
										val_to_str(i, str_range, "Unknown"),
										tvb_get_ntohs(tvb, offset + 3 + element_length * i));
								}
								break;
							}
						case 0x20: /
							{
								element_length = 4;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "+/- %3d ms: %d",
										((2 * i) + 1) * framing_rtp / 2,
										tvb_get_ntohl(tvb, offset + 3 + 4 * i));
								}
								break;
							}
						case 0x2E: /
							{
								static const value_string str_bool[] = {
									{0x00, "False"},
									{0, NULL}
								};
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%s", val_to_str(tvb_get_guint8(tvb, offset + 3), str_bool, "True"));
								break;
							}
						case 0x2F: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", (tvb_get_guint8(tvb, offset + 3) & 0x07));
								break;
							}
						case 0x30: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d", (tvb_get_ntohs(tvb, offset + 3) & 0x0FFF));
								break;
							}
						case 0x31: /
							{
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%d (%d)", tvb_get_guint8(tvb, offset + 3),
									tvb_get_guint8(tvb, offset + 3)>>2);
								break;
							}
						case 0x3D: /
							{
								static const value_string str_range[] = {
									{0, "< 10 %  "},
									{1, "< 20 %  "},
									{2, "< 40 %  "},
									{3, "< 60 %  "},
									{4, ">= 60 % "},
									{0, NULL}
								};
								element_length = 2;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "%s: %d",
										val_to_str(i, str_range, "Unknown"),
										tvb_get_ntohs(tvb, offset + 3 + element_length * i));
								}
								break;
							}
						case 0x3E: /
							{
								static const value_string str_range[] = {
									{0, "1         "},
									{1, "2         "},
									{2, "3         "},
									{3, "4         "},
									{4, "5 and more"},
									{0, NULL}
								};
								element_length = 2;
								for(i = 0; i < (parameter_length / element_length); i++) {
									g_snprintf(parameter_value_tab[i], 50, "%s: %d",
										val_to_str(i, str_range, "Unknown"),
										tvb_get_ntohs(tvb, offset + 3 + element_length * i));
								}
								break;
							}
						case 0x14: /
							{
								framing_rtp = tvb_get_guint8(tvb, offset + 3);
							}
						case 0x01: /
						case 0x02: /
						case 0x03: /
						case 0x0C: /
						case 0x0D: /
						case 0x0E: /
						case 0x13: /
						case 0x15: /
						case 0x16: /
						case 0x17: /
						case 0x18: /
						case 0x19: /
						case 0x1A: /
						case 0x1C: /
						case 0x1D: /
						case 0x21: /
						case 0x29: /
						case 0x2A: /
						case 0x2D: /
						default:
							{
								guint64 param_value = 0;

								for(i = parameter_length; i > 0; i--) {
									param_value += (tvb_get_guint8(tvb, offset + 2 + i) << (8 * (parameter_length - i)));
								}
								ep_strbuf_truncate(strbuf, 0);
								ep_strbuf_append_printf(strbuf, "%" G_GINT64_MODIFIER "u", param_value);
								break;
							}
						}
					}

					switch(parameter_id)
					{
					/
					case 0x1B:
					case 0x1E:
					case 0x1F:
					case 0x20:
					case 0x3D:
					case 0x3E:
						{
							ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
								parameter_length + 3, "%s:",
								val_to_str(parameter_id, str_parameter_id, "Unknown"));
							proto_tree_add_text(ua3g_body_tree, tvb, offset + 3,
								parameter_length, "          %s",
								val_to_str(parameter_id, str_parameter_id_tab, "Unknown"));
							ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

							proto_tree_add_text(ua3g_param_tree, tvb, offset, 1,
								"Parameter: %s (0x%02x)",
								val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
							offset++;
							length--;

							proto_tree_add_text(ua3g_param_tree, tvb, offset, 2,
								"Length: %d", parameter_length);
							offset += 2;
							length -= 2;

							for(i = 0; i < (parameter_length / element_length); i++) {
								proto_tree_add_text(ua3g_body_tree, tvb, offset,
									element_length, "          %s", parameter_value_tab[i]);
								offset += element_length;
								length -= element_length;
							}
							break;
						}
					default:
						{
							if(parameter_length > 0)
								ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
									parameter_length + 3, "%s: %s",
									val_to_str(parameter_id, str_parameter_id, "Unknown"), strbuf->str);
							else
								ua3g_param_item = proto_tree_add_text(ua3g_body_tree, tvb, offset,
									parameter_length + 3, "%s",
									val_to_str(parameter_id, str_parameter_id, "Unknown"));

							ua3g_param_tree = proto_item_add_subtree(ua3g_param_item, ett_ua3g_param);

							proto_tree_add_text(ua3g_param_tree, tvb, offset, 1, "Parameter: %s (0x%02x)",
								val_to_str(parameter_id, str_parameter_id, "Unknown"), parameter_id);
							offset++;
							length--;

							proto_tree_add_text(ua3g_param_tree, tvb, offset, 2, "Length: %d", parameter_length);
							offset += 2;
							length -= 2;

							if(parameter_length > 0) {
								proto_tree_add_text(ua3g_param_tree, tvb, offset, parameter_length,
									"Value: %s", strbuf->str);
								offset += parameter_length;
								length -= parameter_length;
							}
							break;
						}
					}

				}
				break;
			}
		default:
		{
			break;
		}
	}
}
