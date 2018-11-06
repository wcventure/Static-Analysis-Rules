static void dissect_tns_control(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *control_tree = NULL, *ti;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Control");
		control_tree = proto_item_add_subtree(ti, ett_tns_control);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_control, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Control");
	}

	if ( control_tree )
	{
		proto_tree_add_item(control_tree, hf_tns_control_cmd, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( control_tree )
	{
		proto_tree_add_item(control_tree, hf_tns_control_data, tvb,
			offset, tvb_length_remaining(tvb, offset), FALSE);
	}
	return;
}
