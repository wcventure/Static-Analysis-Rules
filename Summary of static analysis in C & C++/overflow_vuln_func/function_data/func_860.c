static void
fComplexAcknowledge(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{	/
	/
	int offset = 0;
	proto_item *tc, *tt, *ti;
	proto_tree *bacapp_tree, *bacapp_tree_control, *bacapp_tree_tag;
	guint8 tmp, bacapp_type;

	tmp = tvb_get_guint8(tvb, offset);
	bacapp_type = (tmp >> 4) & 0x0f;
	bacapp_flags = tmp & 0x0f;

	tmp = tvb_get_guint8(tvb, offset+2);
	if (bacapp_flags & 0x08)
		tmp = tvb_get_guint8(tvb, offset+4);

	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_str(pinfo->cinfo, COL_INFO, val_to_str(tmp, bacapp_confirmed_service_choice, "Reserved by ASHRAE"));

	if (tree) {

		ti = proto_tree_add_item(tree, proto_bacapp, tvb, offset, -1, FALSE);
		bacapp_tree = proto_item_add_subtree(ti, ett_bacapp);

		tc = proto_tree_add_item(bacapp_tree, hf_bacapp_type, tvb, offset, 1, TRUE);
		bacapp_tree_control = proto_item_add_subtree(tc, ett_bacapp);

		proto_tree_add_item(bacapp_tree, hf_bacapp_SEG, tvb, offset, 1, TRUE);
		proto_tree_add_item(bacapp_tree, hf_bacapp_MOR, tvb, offset++, 1, TRUE);
		proto_tree_add_item(bacapp_tree, hf_bacapp_invoke_id, tvb,
			offset++, 1, TRUE);
		if (bacapp_flags & 0x08) {
			bacapp_seq = tvb_get_guint8(tvb, offset);
			proto_tree_add_item(bacapp_tree, hf_bacapp_sequence_number, tvb,
				offset++, 1, TRUE);
			proto_tree_add_item(bacapp_tree, hf_bacapp_window_size, tvb,
				offset++, 1, TRUE);
		}
		tmp = tvb_get_guint8(tvb, offset);
		tt = proto_tree_add_item(bacapp_tree, hf_bacapp_service, tvb,
			offset++, 1, TRUE);
		/
		bacapp_tree_tag = proto_item_add_subtree(tt, ett_bacapp_tag);
		fTags (tvb, pinfo, bacapp_tree_tag, &offset, tmp); /
	}
}
