static void
dissect_ua3g(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_item *ua3g_item = NULL;
	proto_tree *ua3g_tree = NULL;
	proto_item *ua3g_body_item = NULL;
	gint length = 0;
	gint offset = 0;
	guint8 opcode = 0x00;

	if(tree) {
		ua3g_item = proto_tree_add_item(tree, proto_ua3g, tvb, 0, -1, ENC_NA);
		ua3g_tree = proto_item_add_subtree(ua3g_item, ett_ua3g);

		if(message_direction == SYS_TO_TERM) {
			opcodes_vals = opcodes_vals_sys;
		} else
			opcodes_vals = opcodes_vals_term;

		/
		length = tvb_get_letohs(tvb, offset);

		/
		if (check_col(pinfo->cinfo, COL_INFO))
			col_append_fstr(pinfo->cinfo, COL_INFO, " - UA3G Message:");

		proto_tree_add_uint(ua3g_tree, hf_ua3g_length, tvb, offset, 2, length);
		offset += 2;

		/
		opcode = tvb_get_guint8(tvb, offset);
		if(opcode != 0x9f)
			opcode = (opcode & 0x7f);

		/
		proto_tree_add_uint_format(ua3g_tree, hf_ua3g_opcode, tvb, offset,
			1, opcode, "Opcode: %s (0x%02x)",
			val_to_str(opcode, opcodes_vals, "Unknown"), opcode);
		offset++;
		length--;

		/
		if (check_col(pinfo->cinfo, COL_INFO))
			col_append_fstr(pinfo->cinfo, COL_INFO, " %s",
				val_to_str(opcode, opcodes_vals, "Unknown"));

		proto_item_append_text(ua3g_item, ", %s", val_to_str(opcode, opcodes_vals, "Unknown"));

		if(length > 0)
			ua3g_body_item = proto_tree_add_text(ua3g_tree, tvb, offset,
				length, "UA3G Body - %s",
				val_to_str(opcode, opcodes_vals, "Unknown"));

		if(message_direction == SYS_TO_TERM) {
			switch(opcode) {
			case PRODUCTION_TEST: /
			case SUBDEVICE_RESET: /
			case ARE_YOU_THERE: /
			case SET_SPEAKER_VOL: /
			case TRACE_ON: /
				{
					decode_with_one_parameter(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SUBDEVICE_ESCAPE: /
				{
					decode_subdevice_escape(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SOFT_RESET: /
				{
					decode_software_reset(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case IP_PHONE_WARMSTART: /
				{
					decode_ip_phone_warmstart(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SUPER_MSG: /
			case SUPER_MSG_2: /
				{
					decode_super_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SEGMENT_MSG: /
				{
					decode_segment_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case IP_DEVICE_ROUTING: /
				{
					decode_ip_device_routing(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case DEBUG_IN_LINE: /
				{
					decode_debug_in_line(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case LED_COMMAND: /
				{
					decode_led_command(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case LCD_LINE_1_CMD: /
			case LCD_LINE_2_CMD: /
				{
					decode_lcd_line_cmd(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case MAIN_VOICE_MODE: /
				{
					decode_main_voice_mode(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case SUBDEVICE_METASTATE: /
				{
					decode_subdevice_metastate(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case DWL_DTMF_CLCK_FORMAT: /
				{
					decode_dwl_dtmf_clck_format(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SET_CLCK: /
				{
					decode_set_clck(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case VOICE_CHANNEL: /
				{
					decode_voice_channel(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case EXTERNAL_RINGING: /
				{
					decode_external_ringing(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case LCD_CURSOR: /
				{
					decode_lcd_cursor(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case DWL_SPECIAL_CHAR: /
				{
					decode_dwl_special_char(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SET_CLCK_TIMER_POS: /
				{
					decode_set_clck_timer_pos(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case SET_LCD_CONTRAST: /
				{
					decode_set_lcd_contrast(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case BEEP: /
				{
					decode_beep(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case SIDETONE: /
				{
					decode_sidetone(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case RINGING_CADENCE: /
				{
					decode_ringing_cadence(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case MUTE: /
				{
					decode_mute(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case FEEDBACK: /
				{
					decode_feedback(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case READ_PERIPHERAL: /
			case WRITE_PERIPHERAL: /
				{
					decode_r_w_peripheral(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case ICON_CMD: /
				{
					decode_icon_cmd(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case AUDIO_CONFIG: /
				{
					decode_audio_config(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case AUDIO_PADDED_PATH: /
				{
					decode_audio_padded_path(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case KEY_RELEASE: /
			case AMPLIFIED_HANDSET: /
			case LOUDSPEAKER: /
			case ANNOUNCE: /
				{
					decode_on_off_level(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case RING: /
				{
					decode_ring(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case UA_DWL_PROTOCOL: /
				{
					decode_ua_dwl_protocol(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			/
			case NOP: /
			case HE_ROUTING:	/
			case LOOPBACK_ON: /
			case LOOPBACK_OFF: /
			case VIDEO_ROUTING:	/
			case REMOTE_UA_ROUTING:	/
			case VERY_REMOTE_UA_ROUTING:	/
			case OSI_ROUTING:	/
			case ABC_A_ROUTING:	/
			case IBS_ROUTING:	/
			case M_REFLEX_HUB_ROUTING:	/
			case START_BUZZER: /
			case STOP_BUZZER: /
			case ENABLE_DTMF: /
			case DISABLE_DTMF: /
			case CLEAR_LCD_DISP: /
			case VERSION_INQUIRY: /
			case VTA_STATUS_INQUIRY: /
			case SUBDEVICE_STATE: /
			case AUDIO_IDLE: /
			case TRACE_OFF: /
			case ALL_ICONS_OFF: /
			case RELEASE_RADIO_LINK: /
			case DECT_HANDOVER:	/
			default:
				{
					break;
				}
			}
		}
		if(message_direction == TERM_TO_SYS) {
			switch(opcode) {
			case CS_DIGIT_DIALED: /
				{
					decode_digit_dialed(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_SUBDEVICE_MSG: /
				{
					decode_subdevice_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_SUPER_MSG: /
			case CS_SUPER_MSG_2: /
				{
					decode_super_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_SEGMENT_MSG: /
				{
					decode_segment_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_IP_DEVICE_ROUTING: /
				{
					decode_cs_ip_device_routing(ua3g_tree, tvb, pinfo, offset, length, opcode,ua3g_item, ua3g_body_item);
					break;
				}
			case CS_DEBUG_IN_LINE: /
				{
					decode_debug_in_line(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_NON_DIGIT_KEY_PUSHED:	/
			case CS_DIGIT_KEY_RELEASED:	/
			case CS_KEY_RELEASED: /
			case CS_TM_KEY_PUSHED:	/
				{
					decode_key_number(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_UNSOLICITED_MSG: /
			case CS_VERSION_RESPONSE: /
				{
					decode_unsolicited_msg(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			case CS_I_M_HERE: /
				{
					decode_i_m_here(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_RSP_STATUS_INQUIRY: /
			case CS_SPECIAL_KEY_STATUS: /
				{
					decode_special_key(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_SUBDEVICE_STATE: /
				{
					decode_subdevice_state(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_PERIPHERAL_CONTENT: /
				{
					decode_r_w_peripheral(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_body_item);
					break;
				}
			case CS_UA_DWL_PROTOCOL: /
				{
					decode_ua_dwl_protocol(ua3g_tree, tvb, pinfo, offset, length, opcode, ua3g_item, ua3g_body_item);
					break;
				}
			/
			case CS_NOP_ACK: /
			case CS_HANDSET_OFFHOOK: /
			case CS_HANDSET_ONHOOK: /
			case CS_HE_ROUTING:	/
			case CS_LOOPBACK_ON: /
			case CS_LOOPBACK_OFF: /
			case CS_VIDEO_ROUTING:	/
			case CS_WARMSTART_ACK: /
			case CS_REMOTE_UA_ROUTING:	/
			case CS_VERY_REMOTE_UA_R:	/
			case CS_OSI_ROUTING:	/
			case CS_ABC_A_ROUTING:	/
			case CS_IBS_ROUTING:	/
			case CS_TRACE_ON_ACK: /
			case CS_TRACE_OFF_ACK: /
			default:
				{
					break;
				}
			}
		}
	}
}
