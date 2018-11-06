static void
decode_debug_in_line(proto_tree *tree _U_, tvbuff_t *tvb, packet_info *pinfo _U_,
		     guint offset, guint length, guint8 opcode _U_,
		     proto_item *ua3g_body_item)
{
	gint *lengthp = 0;
	proto_tree *ua3g_body_tree;
	int i, parameter_length;
	emem_strbuf_t *strbuf = ep_strbuf_new_label("");

	parameter_length = length;
	ua3g_body_tree = proto_item_add_subtree(ua3g_body_item, ett_ua3g_body);

	ep_strbuf_truncate(strbuf, 0);
	ep_strbuf_append(strbuf, "\"");
	for(i = 0; i < parameter_length; i++) {
		if(isprint(tvb_get_guint8(tvb, offset + i)))
			ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+i, lengthp));
		else
			ep_strbuf_append(strbuf, tvb_get_const_stringz(tvb, offset+i, lengthp));
	}
	ep_strbuf_append(strbuf, "\"");

	proto_tree_add_text(ua3g_body_tree, tvb, offset, length,
		"Text String With Debug: %s", strbuf->str);
}
