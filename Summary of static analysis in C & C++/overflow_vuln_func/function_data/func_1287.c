static void dissect_tns_redirect(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *redirect_tree = NULL, *ti;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Redirect");
		redirect_tree = proto_item_add_subtree(ti, ett_tns_redirect);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_redirect, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Redirect");
	}

	if ( redirect_tree )
	{
		proto_tree_add_item(redirect_tree, hf_tns_redirect_data_length, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( redirect_tree )
	{
		proto_tree_add_item(redirect_tree, hf_tns_redirect_data, tvb,
			offset, tvb_length_remaining(tvb, offset), FALSE);
	}
	return;
}
