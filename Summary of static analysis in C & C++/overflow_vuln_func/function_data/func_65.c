static void
decode_ua_dwl_protocol(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo,
		       guint offset, guint length, guint8 opcode _U_,
		       proto_item *ua3g_item, proto_item *ua3g_body_item)
{
	guint8 command = tvb_get_guint8(tvb, offset);
	proto_tree *ua3g_body_tree;
	static const value_string str_command[] = {
		{0x00, "Downloading Suggest"},
		{0x01, "Downloading Request"},
		{0x02, "Downloading Acknowledge"},
		{0x03, "Downloading Data"},
		{0x04, "Downloading End"},
		{0x05, "Downloading End Acknowledge"},
		{0x06, "Downloading ISO Checksum"},
		{0x07, "Downloading ISO Checksum Acknowledge"},
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
		offset, 1, command, "Command: %s",
		val_to_str(command, str_command, "Unknown"));
	offset++;
	length--;

	switch(command) {
	case 0x00:	/
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Item Identifier: %d", tvb_get_guint8(tvb, offset));
			offset++;
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Item Version: %s",
				version_number_computer(tvb_get_letohs(tvb, offset)));
			offset += 2;
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Cause: %d", tvb_get_guint8(tvb, offset));
			break;
		}
	case 0x01:	/
		{
			static const value_string str_force_mode[] = {
				{0x00, "System Accept All Refusals"},
				{0x01, "Force Software Lock"},
				{0, NULL}
			};
			static const value_string str_item_id[] = {
				{0x00, "Patches File"},
				{0x01, "Application Binary"},
				{0x02, "Datas Binary"},
				{0, NULL}
			};
			static const value_string str_mode_selection_country[] = {
				{0x00, "No Check"},
				{0x01, "For All Countries Except Chinese"},
				{0x02, "For Chinese"},
				{0, NULL}
			};
			static const value_string str_mem_size[] = {
				{0x00, "No Check"},
				{0x01, "128 Kbytes"},
				{0x02, "256 Kbytes"},
				{0x03, "512 Kbytes"},
				{0x04, "1 Mbytes"},
				{0x05, "2 Mbytes"},
				{0x06, "4 Mbytes"},
				{0x07, "8 Mbytes"},
				{0, NULL}
			};
			static const value_string str_bin_info[] = {
				{0x00, "Uncompressed Binary"},
				{0x01, "LZO Compressed Binary"},
				{0, NULL}
			};

			if(length > 7) { /
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Force Mode: %s",
					val_to_str(tvb_get_guint8(tvb, offset), str_force_mode, "Unknown"));
				offset++;
				length--;
			}

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Item Identifier: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_item_id, "Unknown"));
			offset++;
			length--;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
				"Item Version: %d", tvb_get_ntohs(tvb, offset));
			offset += 2;
			length -= 2;

			if(length > 2) { /
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Files Included: %s Boot Binary Included, %s Loader Binary Included, %s Appli Binary Included, %s Datas Binary Included",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_yes_no, ""),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_yes_no, ""),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x04), str_yes_no, ""),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x08), str_yes_no, ""));
				offset++;
				length--;

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Model Selection: For A Model: %s, For B Model: %s, For C Model %s, Country Version: %s",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_yes_no, "Yes"),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_yes_no, "Yes"),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x04), str_yes_no, "Yes"),
					val_to_str(((tvb_get_guint8(tvb, offset) & 0xE0) >> 5), str_mode_selection_country, "Unknown"));
				offset++;
				length--;

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Hardware Selection: For Ivanoe 1: %s, For Ivanoe 2: %s",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_yes_no, "Yes"),
					val_to_str((tvb_get_guint8(tvb, offset) & 0x02), str_yes_no, "Yes"));
				offset++;
				length--;

				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Memory Sizes Required: Flash Min Size: %s, External Ram Min Size: %s",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x07), str_mem_size, "Unknown"),
					val_to_str(((tvb_get_guint8(tvb, offset) & 0x38) >> 3), str_mem_size, "Unknown"));
				offset++;
				length--;
			} else { /
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Binary Information: %s, Country/Operator/CLient Identifier ?",
					val_to_str((tvb_get_guint8(tvb, offset) & 0x01), str_bin_info, ""));
				offset++;
				length--;
			}

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 3,
				"Binary Length: %d", tvb_get_ntoh24(tvb, offset));
			break;
		}
	case 0x02:	/
		{
			static const value_string str_status[] = {
				{0x00, "Ok (Binary Item Downloading In \"Normal\" Progress)"},
				{0x01, "Hardware Failure: Flash Failure"},
				{0x02, "Not Enough Place To Store The Downloaded Binary"},
				{0x03, "Wrong Seq Number On Latest Received Download_Data Message"},
				{0x04, "Wrong Packet Number On Latest Received Download_Data Message"},
				{0x05, "Download Refusal Terminal (Validation Purpose)"},
				{0x06, "Download Refusal Terminal (Development Purpose)"},
				{0x10, "Download Refusal: Hardware Cause (Unknown Flash Device, Incompatible Hardware)"},
				{0x11, "Download Refusal: No Loader Available Into The Terminal"},
				{0x12, "Download Refusal: Software Lock"},
				{0x13, "Download Refusal: Wrong Parameter Into Download Request"},
				{0x20, "Wrong Packet Number On Latest Received Downloading_Data Message"},
				{0x21, "Compress Header Invalid"},
				{0x22, "Decompress Error"},
				{0x23, "Binary Header Invalid"},
				{0x24, "Binary Check Error: Flash Write Error Or Binary Is Invalid"},
				{0x25, "Error Already Signaled - No More Data Accepted"},
				{0x26, "No Downloading In Progress"},
				{0x27, "Too Many Bytes Received (More Than Size Given Into The Download_Req Message)"},
				{0xFF, "Undefined Error"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
				"Packet Number: %d", tvb_get_ntohs(tvb, offset));
			offset += 2;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Status: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_status, "Unknown"));
			break;
		}
	case 0x03:	/
		{
			int i = 1;

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 2,
				"Packet Number: %d", tvb_get_ntohs(tvb, offset));
			offset += 2;
			length -= 2;

			while(length > 0) {
				proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
					"Packet Number %3d: %d", i, tvb_get_guint8(tvb, offset));
				offset++;
				length--;
				i++;
			}
			break;
		}
	case 0x05:	/
		{
			static const value_string str_ok[] = {
				{0x00, "Ok"},
				{0x01, "Hardware Failure: Flash Problems"},
				{0x02, "Not Enough Place To Store The Downloaded Binary"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Status: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_ok, "Not Ok"));
			break;
		}
	case 0x06:	/
		{
			proto_tree_add_text(ua3g_body_tree, tvb, offset, 4,
				"Checksum: %d", tvb_get_ntohl(tvb, offset));
			/
			/
			/
			break;
		}
	case 0x07:	/
		{
			static const value_string str_ack_status[] = {
				{0x00, "The Checksum Matches"},
				{0x25, "Error Detected And Already Signaled"},
				{0x30, "Checksum Error (All Bytes Received)"},
				{0x31, "Checksum Error (Bytes Missing)"},
				{0, NULL}
			};

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1, "Acknowledge: %s",
				val_to_str(tvb_get_guint8(tvb, offset), str_ack_status, "Unknown"));
			break;
		}
	case 0x04:	/
	default:
		{
			break;
		}
	}
}
