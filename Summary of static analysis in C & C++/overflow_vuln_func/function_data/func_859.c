static void
fSimpleAcknowledge(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{	/
	/
	int offset = 0;
	proto_item *tc, *ti;
	guint8 tmp;
	proto_tree *bacapp_tree;

	tmp = tvb_get_guint8(tvb, offset+2);
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_str(pinfo->cinfo, COL_INFO, val_to_str(tmp, bacapp_confirmed_service_choice, "Reserved by ASHRAE"));

	if (tree) {
		ti = proto_tree_add_item(tree, proto_bacapp, tvb, offset, -1, FALSE);
		bacapp_tree = proto_item_add_subtree(ti, ett_bacapp);

		tc = proto_tree_add_item(bacapp_tree, hf_bacapp_type, tvb, offset++, 1, TRUE);

		proto_tree_add_item(bacapp_tree, hf_bacapp_invoke_id, tvb,
			offset++, 1, TRUE);
		proto_tree_add_item(bacapp_tree, hf_bacapp_service, tvb,
			offset++, 1, TRUE);
	}
}
