static void dissect_tns_abort(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *abort_tree = NULL, *ti;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Abort");
		abort_tree = proto_item_add_subtree(ti, ett_tns_abort);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_abort, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Abort");
	}

	if ( abort_tree )
	{
		proto_tree_add_item(abort_tree, hf_tns_abort_reason_user, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( abort_tree )
	{
		proto_tree_add_item(abort_tree, hf_tns_abort_reason_system, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( abort_tree )
	{
		proto_tree_add_item(abort_tree, hf_tns_abort_data, tvb,
			offset, tvb_length_remaining(tvb,offset), FALSE);
	}
	return;
}
