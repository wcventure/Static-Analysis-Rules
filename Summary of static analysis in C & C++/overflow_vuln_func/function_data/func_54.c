static void
decode_beep(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
	    guint offset, guint length, guint8 opcode _U_,
	    proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	emem_strbuf_t *strbuf = ep_strbuf_new_label("");
	if(length > 0) { /
		guint8 command = tvb_get_guint8(tvb, offset);
		proto_tree *ua3g_body_tree;
		static const value_string str_command[] = {
			{0x01, "Beep Once"},
			{0x02, "Beep Start"},
			{0x03, "Stop Beep"},
			{0x04, "Start Beep"},
			{0x05, "Define Beep"},
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
			offset, 1, command, "Beep: %s",
			val_to_str(command, str_command, "Unknown"));
		offset++;
		length--;

		switch(command) {
		case 0x01: /
		case 0x02: /
			{
				int i =  0;
				static const value_string str_destination[] = {
					{0x01, "Ear-Piece"},
					{0x02, "Loudspeaker"},
					{0x02, "Ear-Piece and Loudspeaker"},
					{0, NULL}
				};
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Destination: %s",
					val_to_str(tvb_get_guint8(tvb, offset), str_destination, "Unknown"));
				offset++;
				length--;

				while(length > 0) {
					i++;
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "On / Off: %s",
						val_to_str((tvb_get_guint8(tvb, offset) & 0x80), str_on_off, "On"));
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Cadence T%d: %d ms",
						i, ((tvb_get_guint8(tvb, offset) & 0x7F) * 10));
					offset++;
					length--;
				}
				break;
			}
		case 0x04: /
			{
				int i;
				static const value_string str_destination[] = {
					{0x01, "Handset"},
					{0x02, "Headset"},
					{0x04, "Loudspeaker"},
					{0x08, "Announce Loudspeaker"},
					{0x10, "Handsfree"},
					{0, NULL}
				};

				ep_strbuf_append_printf(strbuf, "%s",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_destination, ""));
				for(i = 1; i < 5; i++) {
					ep_strbuf_append(strbuf,
						val_to_str((tvb_get_guint8(tvb, offset) & (0x01 << i)), str_destination, ""));
				}

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Destination: %s", strbuf->str);
				offset++;

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Beep Number: %x", tvb_get_guint8(tvb, offset));
				break;
			}
		case 0x05:
			{
				int i, nb_of_notes, beep_number;
				static const value_string str_freq_sample_nb[] = {
					{0x00, "Frequency"},
					{0xFF, "Audio Sample Number"},
					{0, NULL}
				};
				static const value_string str_duration[] = {
					{0x00, "Duration "},
					{0xFF, "Duration (Ignored)"},
					{0, NULL}
				};
				static const value_string str_terminator[] = {
					{0xFD, "Stop"},
					{0xFE, "Loop"},
					{0xFF, "Infinite"},
					{0, NULL}
				};

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Beep Number: %x", beep_number = tvb_get_guint8(tvb, offset));
				offset++;
				length--;

				if(beep_number <= 0x44)
					beep_number = 0x00;
				else beep_number = 0xFF;

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Number Of Notes: %x", nb_of_notes = tvb_get_guint8(tvb, offset));
				offset++;
				length--;

				while(length > 0) {
					for(i = 1; i <= nb_of_notes; i++) {
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s %d: %d",
							val_to_str(beep_number, str_freq_sample_nb, "Unknown"),
							i, tvb_get_guint8(tvb, offset));
						offset++;
						length--;
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Level %d: %d",
							i, tvb_get_guint8(tvb, offset));
						offset++;
						length--;
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "%s %d: %x",
							val_to_str(beep_number, str_duration, "Unknown"),
							i, tvb_get_guint8(tvb, offset));
						offset++;
						length--;
					}
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Terminator: %d (%s)",
						tvb_get_guint8(tvb, offset),
						val_to_str(tvb_get_guint8(tvb, offset), str_terminator, "Unknown"));
					offset++;
					length--;
				}
				break;
			}
		case 0x03: /
		default:
			{
				break;
			}
		}
	} else { /
		/
		proto_item_append_text(ua3g_item, ", Classical Beep");
		proto_item_append_text(ua3g_body_item, " - Classical Beep");

		/
		if (check_col(pinfo->cinfo, COL_INFO))
			col_append_fstr(pinfo->cinfo, COL_INFO, ": Classical Beep");
	}
}
