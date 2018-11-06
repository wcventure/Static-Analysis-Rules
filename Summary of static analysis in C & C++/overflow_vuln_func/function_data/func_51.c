static void
decode_dwl_special_char(proto_tree *tree _U_, tvbuff_t *tvb,
			packet_info *pinfo _U_, guint offset, guint length,
			guint8 opcode _U_, proto_item *ua3g_body_item)
{
	proto_tree *ua3g_body_tree;
	int i, j;
	emem_strbuf_t *strbuf = ep_strbuf_new_label("");

	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	while(length > 0) {
		proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
			"Character Number: %d", tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		for(i = 1; i <= 8; i++) {
			int byte = tvb_get_guint8(tvb, offset);
			char byte_char[20] = "  ";

			/
			for(j = 7; j >= 0; j--) {
				if(((byte >> j) & 0x01) == 0)
					ep_strbuf_append_printf(strbuf, "  ");
				else
					ep_strbuf_append_printf(strbuf, "o ");
			}

			proto_tree_add_text(ua3g_body_tree, tvb, offset, 1,
				"Byte %d: 0x%02x   %s", i, byte, byte_char);
			offset++;
			length--;
		}
	}
}
