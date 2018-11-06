static void dissect_noe(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_item    *noe_item  = NULL;
	proto_tree    *noe_tree  = NULL;
	gint           length    = 0;
	guint8         server    = 0;
	gint           offset    = 0;
	guint8         method    = 0xff;
	guint8         methodack = 0;

	(void)pinfo;

	if(tree)
	{
		noe_item = proto_tree_add_item(tree, proto_noe, tvb, 0, -1, ENC_NA);
		noe_tree = proto_item_add_subtree(noe_item, ett_noe);

		length = tvb_get_letohs(tvb, offset);

		proto_tree_add_uint(noe_tree,
			hf_noe_length,
			tvb,
			offset,
			2,
			length);
		offset += 2;

		server = tvb_get_guint8(tvb, offset);

		/
		if(check_col(pinfo->cinfo, COL_INFO))
			col_append_fstr(pinfo->cinfo, COL_INFO, " - NOE Protocol (%s)",
			val_to_str(server, VALS(servers_short_vals), "Unknown"));
		
		proto_tree_add_uint(noe_tree,
			hf_noe_server,
			tvb,
			offset,
			1,
			tvb_get_guint8(tvb, offset));
		offset++;
		length--;
		
		/
		proto_item_append_text(noe_item, ", %s",
			val_to_str(server, VALS(servers_short_vals), "Unknown"));

		method    = tvb_get_guint8(tvb, offset);
		methodack = (method & 0x80) ? 1 : 0;
		method    = (method & 0x7f);

		proto_tree_add_uint_format_value(noe_tree,
			hf_noe_method,
			tvb,
			offset,
			1,
			method,
			"%s (%d)",
			val_to_str(method, VALS(methods_vals), "Unknown"),
			method);

		if(method >= METHOD_INVALID)
			return;

		/
		if(check_col(pinfo->cinfo, COL_INFO))
			col_append_fstr(pinfo->cinfo, COL_INFO, ": %s",
			val_to_str(method, VALS(methods_vals), "Unknown"));

		/
		proto_item_append_text(noe_item, ", %s",
			val_to_str(method, VALS(methods_vals), "Unknown"));

		if(method == METHOD_NOTIFY)
		{
			offset++;
			length--;
			decode_evt(noe_tree, tvb, pinfo, offset, length);
		}
		else
		/
		{
			proto_tree_add_boolean(noe_tree,
				hf_noe_method_ack,
				tvb,
				offset,
				1,
				methodack);
			offset++;
			length--;
			decode_mtd(noe_tree, tvb, pinfo, method, offset, length);
		}
	}
}
