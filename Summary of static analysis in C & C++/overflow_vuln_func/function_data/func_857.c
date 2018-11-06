static void
fConfirmedServiceRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{	/
	/
	int offset = 0;
	proto_item *tc, *tt, *ti;
	proto_tree *bacapp_tree, *bacapp_tree_control, *bacapp_tree_tag;
	guint8 tmp, bacapp_type, service_choice;

	tmp = tvb_get_guint8(tvb, offset);
	bacapp_type = (tmp >> 4) & 0x0f;
	bacapp_flags = tmp & 0x0f;

	service_choice = tvb_get_guint8(tvb, offset+3);
	if (bacapp_flags & 0x08)
		service_choice = tvb_get_guint8(tvb, offset+5);


	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_str(pinfo->cinfo, COL_INFO, val_to_str(service_choice, bacapp_confirmed_service_choice, "Reserved by ASHRAE"));

	if (tree) {
		ti = proto_tree_add_item(tree, proto_bacapp, tvb, offset, -1, FALSE);
		bacapp_tree = proto_item_add_subtree(ti, ett_bacapp);

		tc = proto_tree_add_item(bacapp_tree, hf_bacapp_type, tvb, offset, 1, TRUE);
		bacapp_tree_control = proto_item_add_subtree(tc, ett_bacapp);

		proto_tree_add_item(bacapp_tree_control, hf_bacapp_SEG, tvb, offset, 1, TRUE);
		proto_tree_add_item(bacapp_tree_control, hf_bacapp_MOR, tvb, offset, 1, TRUE);
		proto_tree_add_item(bacapp_tree_control, hf_bacapp_SA, tvb, offset++, 1, TRUE);
		proto_tree_add_item(bacapp_tree_control, hf_bacapp_response_segments, tvb,
			                offset, 1, TRUE);
		proto_tree_add_item(bacapp_tree_control, hf_bacapp_max_adpu_size, tvb,
							offset, 1, TRUE);
		offset ++;
		proto_tree_add_item(bacapp_tree, hf_bacapp_invoke_id, tvb,	offset++, 1, TRUE);
		if (bacapp_flags & 0x08) {
			bacapp_seq = tvb_get_guint8(tvb, offset);
			proto_tree_add_item(bacapp_tree_control, hf_bacapp_sequence_number, tvb,
				offset++, 1, TRUE);
			proto_tree_add_item(bacapp_tree_control, hf_bacapp_window_size, tvb,
				offset++, 1, TRUE);
		}
		tmp = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(bacapp_tree, hf_bacapp_service, tvb,
			offset++, 1, TRUE);
		tt = proto_tree_add_item(bacapp_tree, hf_bacapp_vpart, tvb,
			offset, 0, TRUE);
		/
		bacapp_tree_tag = proto_item_add_subtree(tt, ett_bacapp_tag);
		fTagRequests (tvb, pinfo, bacapp_tree_tag, &offset, tmp); /
	}
}
