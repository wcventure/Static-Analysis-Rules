static void dissect_tns_accept(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *accept_tree = NULL, *ti;
	int accept_offset;
	int accept_len;
	int tns_offset = offset-8;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Accept");
		accept_tree = proto_item_add_subtree(ti, ett_tns_accept);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_accept, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Accept");
	}

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_version, tvb,
			offset, 2, FALSE);
	}
	offset += 2;
	
	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_service_options, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_sdu_size, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_max_tdu_size, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_value_of_one, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	accept_len = tvb_get_ntohs(tvb, offset);
	if ( accept_tree )
	{
		proto_tree_add_uint(accept_tree, hf_tns_accept_data_length, tvb,
			offset, 2, accept_len);
	}
	offset += 2;

	accept_offset = tvb_get_ntohs(tvb, offset);
	if ( accept_tree )
	{
		proto_tree_add_uint(accept_tree, hf_tns_accept_data_offset, tvb,
			offset, 2, accept_offset);
	}
	offset += 2;

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_connect_flags0, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( accept_tree )
	{
		proto_tree_add_item(accept_tree, hf_tns_connect_flags1, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( accept_tree && accept_len > 0)
	{
		proto_tree_add_item(accept_tree, hf_tns_accept_data, tvb,
			tns_offset+accept_offset,
			tvb_length_remaining(tvb, tns_offset+accept_offset), FALSE);
	}
	return;
}
