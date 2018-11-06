static void
decode_unsolicited_msg(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		       guint offset, guint length, guint8 opcode,
		       proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_command[] = {
		{0x00, "Hardware Reset Acknowledge"},
		{0x01, "Software Reset Acknowledge"},
		{0x02, "Illegal Command Received"},
		{0x05, "Subdevice Down"},
		{0x06, "Segment Failure"},
		{0x07, "UA Device Event"},
		{0, NULL}
	};

	if(opcode != 0x21) {
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
			offset, 1, command, "Command: %s",
			val_to_str(command, str_command, "Unknown"));
		offset++;
		length--;
	} else {
		ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);
		command = 0xFF; /
	}

	switch(command)
	{
	case 0x00: /
	case 0x01: /
	case 0xFF: /
		{
			int link, vta_type;
			static const value_string str_vta_type[] = {
				{0x03, "4035"},
				{0x04, "4020"},
				{0x05, "4010"},
				{0x20, "NOE A"},
				{0x21, "NOE B"},
				{0x22, "NOE C"},
				{0x23, "NOE D"},
				{0, NULL}
			};
			static const value_string str_other_info_1[] = {
				{0x00, "Link Is TDM"},
				{0x01, "Link Is IP"},
				{0, NULL}
			};
			static const value_string str_other_info_2[] = {
				{0x00, "Download Allowed"},
				{0x01, "Download Refused"},
				{0, NULL}
			};
			static const value_string str_hard_config_ip[] = {
				{0x00, "Export Binary (No Thales)"},
				{0x01, "Full Binary (Thales)"},
				{0, NULL}
			};
			static const value_string str_hard_config_chip[] = {
				{0x00, "Chip Id: Unknown"},
				{0x01, "Chip Id: Ivanoe 1"},
				{0x02, "Chip Id: Ivanoe 2"},
				{0x03, "Chip Id: Reserved"},
				{0, NULL}
			};
			static const value_string str_hard_config_flash[] = {
				{0x00, "Flash Size: No Flash"},
				{0x01, "Flash Size: 128 Kbytes"},
				{0x02, "Flash Size: 256 Kbytes"},
				{0x03, "Flash Size: 512 Kbytes"},
				{0x04, "Flash Size: 1 Mbytes"},
				{0x05, "Flash Size: 2 Mbytes"},
				{0x06, "Flash Size: 4 Mbytes"},
				{0x07, "Flash Size: 8 Mbytes"},
				{0, NULL}
			};
			static const value_string str_hard_config_ram[] = {
				{0x00, "External RAM Size: No External RAM"},
				{0x01, "External RAM Size: 128 Kbytes"},
				{0x02, "External RAM Size: 256 Kbytes"},
				{0x03, "External RAM Size: 512 Kbytes"},
				{0x04, "External RAM Size: 1 Mbytes"},
				{0x05, "External RAM Size: 2 Mbytes"},
				{0x06, "External RAM Size: 4 Mbytes"},
				{0x07, "External RAM Size: 8 Mbytes"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Device Type: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_device_type, "Unknown"));
			offset++;
			length--;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 2, "Firmware Version: %s",
				version_number_computer(tvb_get_ntohs(tvb, offset)));
			offset += 2;
			length -= 2;

			if(opcode != 0x21) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Self-Test Result: %d", tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}

			vta_type = tvb_get_guint8(tvb, offset);

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "VTA Type: %s",
				val_to_str(vta_type, str_vta_type, "Unknown"));
			offset++;
			length--;

			switch(vta_type)
			{
			case 0x03:
			case 0x04:
			case 0x05:
				{
					static const value_string str_subtype[] = {
						{0x03, "2x40"},
						{0x04, "1x20"},
						{0x05, "1x20"},
						{0, NULL}
					};
					static const value_string str_generation[] = {
						{0x02, "3"},
						{0, NULL}
					};
					static const value_string str_design[] = {
						{0x00, "Alpha"},
						{0, NULL}
					};
					static const value_string str_hard_vta_type[] = {
						{0x03, "MR2 (4035)"},
						{0x05, "VLE (4010)"},
						{0x07, "LE (4020)"},
						{0, NULL}
					};
					static const value_string str_hard_design[] = {
						{0x06, "Alpha"},
						{0, NULL}
					};
					static const value_string str_hard_subtype[] = {
						{0x06, "2x40"},
						{0x07, "1x20"},
						{0x08, "1x20"},
						{0, NULL}
					};

					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
						"Characteristic Number: VTA SubType: %s, Generation: %s, Design: %s",
						val_to_str((((tvb_get_guint8(tvb, offset) & 0xC0) >> 6) + vta_type), str_subtype, "Unknown"),
						val_to_str(((tvb_get_guint8(tvb, offset) & 0x38) >> 3), str_generation, "Unknown"),
						val_to_str((tvb_get_guint8(tvb, offset) & 0x07), str_design, "Unknown"));
					offset++;
					length--;
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
						"Other Information: %s",
						val_to_str(tvb_get_guint8(tvb, offset), str_other_info_2, "Unknown"));
					offset++;
					length--;

					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
						"Hardware Configuration: VTA Type: %s, Design: %s, VTA SubType: %s",
						val_to_str((((tvb_get_guint8(tvb, offset) & 0xE0) >> 5) + vta_type), str_hard_vta_type, "Unknown"),
						val_to_str(((tvb_get_guint8(tvb, offset) & 0x1C) >> 2), str_hard_design, "Unknown"),
						val_to_str((tvb_get_guint8(tvb, offset) & 0x03), str_hard_subtype, "Unknown"));
					offset++;
					length--;

					if(opcode != 0x21) {
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
							"Hook Status/BCM Version: %s Hook",
							val_to_str(tvb_get_guint8(tvb, offset), str_on_off, "On"));
						offset++;
						length--;

					}
					break;
				}
			case 0x20:
			case 0x21:
			case 0x22:
			case 0x23:
			default:
				{
					link = tvb_get_guint8(tvb, offset);
					proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
						"Other Information 1: %s",
						val_to_str(link, str_other_info_1, "Unknown"));
					offset++;
					length--;

					if(link == 0x00) {
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
							"Hardware Version: %d", tvb_get_guint8(tvb, offset));
						offset++;
						length--;

						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
							"Hardware Configuration: %s, %s, %s",
							val_to_str((tvb_get_guint8(tvb, offset) & 0x03), str_hard_config_chip, "Unknown"),
							val_to_str(((tvb_get_guint8(tvb, offset) & 0x1C) >> 2), str_hard_config_flash, "Unknown"),
							val_to_str(((tvb_get_guint8(tvb, offset) & 0xE0) >> 5), str_hard_config_ram, "Unknown"));
						offset++;
						length--;
					} else {
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Other Information 2: %s",
							val_to_str(tvb_get_guint8(tvb, offset), str_other_info_2, "Unknown"));
						offset++;
						length--;

						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Hardware Configuration: %s",
							val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_hard_config_ip, "Unknown"));
						offset++;
						length--;
					}

					if(opcode != 0x21) {
						proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Hook Status: %s Hook",
							val_to_str(tvb_get_guint8(tvb, offset), str_on_off, "On"));
						offset++;
						length--;

						if(length > 0) {
							if(link == 0x00) {
								proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
									"Firmware Datas Patch Version: %s",
									version_number_computer(tvb_get_ntohs(tvb, offset)));
								offset += 2;
								length -= 2;

								if(length > 0) {
									proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
										"Firmware Version (Loader): %s",
										version_number_computer(tvb_get_ntohs(tvb, offset)));
								}
							} else {
								proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
									"Datas Version: %s",
									version_number_computer(tvb_get_ntohs(tvb, offset)));
								offset += 2;
								length -= 2;

								if(length > 0) {
									proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
										"Firmware Version (Bootloader): %s",
										version_number_computer(tvb_get_ntohs(tvb, offset)));
								}
							}
						}
					}
					break;
				}
			}
			break;
		}
	case 0x02: /
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Opcode Of Bad  Command: %d", tvb_get_guint8(tvb, offset));
			offset++;
			length--;

			while(length >0) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Next Byte Of Bad Command: %d", tvb_get_guint8(tvb, offset));
				offset++;
				length--;
			}
			break;
		}
	case 0x05: /
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Subdevice Address: %d", tvb_get_guint8(tvb, offset));
			break;
		}
	case 0x06: /
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "T: %d",
				(tvb_get_guint8(tvb, offset) & 0x01));
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Num: %d",
				(tvb_get_guint8(tvb, offset) & 0x02));
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "/S: %d",
				(tvb_get_guint8(tvb, offset) & 0x04));
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "L: %d",
				(tvb_get_guint8(tvb, offset) & 0x08));
			offset++;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Opcode Bad Segment: %d", tvb_get_guint8(tvb, offset));
			offset++;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Next Byte Of Bad Segment: %d",
				tvb_get_guint8(tvb, offset));
			break;
		}
	case 0x07: /
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Device Event: %d", tvb_get_guint8(tvb, offset));
			break;
		}
	default:
		{
			break;
		}
	}
}
