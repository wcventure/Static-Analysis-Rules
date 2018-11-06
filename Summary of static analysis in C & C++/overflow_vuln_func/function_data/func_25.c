static void uadecode(e_ua_direction direction,
					 proto_tree *tree,
					 packet_info *pinfo,
					 tvbuff_t *tvb,
					 gint offset,
					 gint opcode,
					 gint length)
{
	message_direction = direction;

	switch(opcode & 0x7f) /
	{
	case 0x15:
	case 0x16:
		{
			call_dissector(find_dissector("noe"),
				tvb_new_subset(tvb, offset, length, length),
				pinfo,
				tree);
			break;
		}
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:	/
	case 0x08:	/
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x17:
	case 0x18:
	case 0x1F:	/
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:	/
	case 0x25:	/
	case 0x26:
	case 0x27:
	case 0x28:
	case 0x29:
	case 0x2A:
	case 0x2B:	/
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x30:
	case 0x31:
	case 0x32:	/
	case 0x33:
	case 0x35:
	case 0x36:	/
	case 0x38:
	case 0x39:
	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x3F:
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F:
	case 0x50:	/
		{
			call_dissector(find_dissector("ua3g"),
			tvb_new_subset(tvb, offset, length, length),
			pinfo,
			tree);
			break;
		}
	default:
		{
			/
			if (check_col(pinfo->cinfo, COL_INFO))
				col_append_fstr(pinfo->cinfo, COL_INFO, " - UA3G Message ERR: Opcode Unknown");

			proto_tree_add_text(tree,
				tvb,
				offset,
				length,
				"Opcode Unknown 0x%02x",
				tvb_get_guint8(tvb, (offset + 2)));
			break;
		}
	}
}
