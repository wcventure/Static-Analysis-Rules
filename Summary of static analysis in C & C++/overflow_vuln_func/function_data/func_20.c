static void decode_evt(proto_tree *tree,
					   tvbuff_t *tvb,
					   packet_info *pinfo,
					   guint offset,
					   guint length)
{
	guint8 event = tvb_get_guint8(tvb, offset);

	proto_tree_add_uint_format_value(tree,
		hf_noe_event,
		tvb,
		offset,
		1,
		event,
		"%s (%d)",
		val_to_str(event, val_str_event, "Unknown"),
		event);

	/
	if(check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, " %s",
		val_to_str(event, val_str_event, "Unknown"));
	/
	proto_item_append_text(tree, ", %s",
		val_to_str(event, val_str_event, "Unknown"));

	offset++;
	length--;

	switch(event)
	{
	case OPCODE_EVT_BT_KEY_SHORTPRESS:
	case OPCODE_EVT_BT_KEY_LONGPRESS:
	case OPCODE_EVT_BT_KEY_VERYLONGPRESS:
	case OPCODE_EVT_KEY_LINE:
	case OPCODE_EVT_ONHOOK:
	case OPCODE_EVT_OFFHOOK:
		{
			static const value_string str_struct[] = {
				{0x00, "RJ9 Plug"},
				{0x01, "BT Handset Link"},
				{0, NULL}
				};
			proto_tree_add_text(tree,
				tvb,
				offset,
				length,
				"Value: %s (%d)",
				val_to_str(tvb_get_guint8(tvb, offset), str_struct, "Unknown"), tvb_get_guint8(tvb, offset));
			break;
		}
	case OPCODE_EVT_KEY_PRESS:
	case OPCODE_EVT_KEY_RELEASE:
	case OPCODE_EVT_KEY_SHORTPRESS:
	case OPCODE_EVT_KEY_LONGPRESS:
	case OPCODE_EVT_HELP:
		{
			/
			guint64 utf8_value = 0;
			guint64 unicode_value;
			static char key_name[30];
			int pt_length = length;
			int pt_offset = offset;

			while(pt_length > 0)
			{
				utf8_value = (utf8_value << 8) + tvb_get_guint8(tvb, pt_offset);
				pt_offset ++;
				pt_length --;
			}
			unicode_value = decode_utf8(utf8_value);
			g_snprintf(key_name, 30, "\"%s\"", decode_key_name((int)unicode_value));

			/
			if(check_col(pinfo->cinfo, COL_INFO))
				col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
				key_name);
			/
			proto_item_append_text(tree, ", %s",
				key_name);

			proto_tree_add_text(tree,
				tvb,
				offset,
				length,
				"Key Name: %s (UTF-8 Value: %s, Unicode Value: 0x%" G_GINT64_MODIFIER "x)",
				key_name,
				tvb_bytes_to_str(tvb, offset, length),
				unicode_value);
			break;
		}
	case OPCODE_EVT_ERROR_PROTOCOL:
	case OPCODE_EVT_ERROR_CREATE:
	case OPCODE_EVT_ERROR_DELETE:
	case OPCODE_EVT_ERROR_SET_PROPERTY:
	case OPCODE_EVT_ERROR_GET_PROPERTY:
		{
			decode_evt_error(tree, tvb, offset, length);
			break;
		}
	case OPCODE_EVT_CONTEXT_SWITCH:
		{
			proto_tree_add_text(tree,
				tvb,
				offset,
				1,
				"Context: %s",
				val_to_str(tvb_get_guint8(tvb, offset),
				VALS(servers_vals),
				"Unknown"));
			break;
		}
	case OPCODE_EVT_SUCCESS_CREATE:
	case OPCODE_EVT_SUCCESS_DELETE:
	case OPCODE_EVT_SUCCESS_SET_PROPERTY:
	case OPCODE_EVT_SUCCESS_INSERT_ITEM:
	case OPCODE_EVT_SUCCESS_DELETE_ITEM:
		{
			proto_tree_add_uint(tree, hf_noe_objectid, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
			break;
		}
	case OPCODE_EVT_WIDGETS_GC:
		{
			proto_tree_add_text(tree,
				tvb,
				offset,
				4,
				"FreeMem: %d bytes",
				tvb_get_ntohl(tvb, offset));
			break;
		}
	case OPCODE_EVT_BT_BONDING_RESULT:
		{
			proto_tree_add_uint(tree, hf_noe_objectid, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
			offset += 2;
			/
			/

			proto_tree_add_text(tree,
				tvb,
				offset,
				1,
				"Bonded: %d",
				tvb_get_ntohs(tvb, offset));
			offset += 1;
			/
			/

			proto_tree_add_text(tree,
				tvb,
				offset,
				1,
				"Value: %d",
				tvb_get_ntohs(tvb, offset));
			break;
		}
	default:
		{
			proto_tree_add_uint(tree, hf_noe_objectid, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
			offset += 2;
			length -= 2;

			if(length > 0)
				decode_tlv(tree, tvb, offset, length);
			break;
		}
	}
}
