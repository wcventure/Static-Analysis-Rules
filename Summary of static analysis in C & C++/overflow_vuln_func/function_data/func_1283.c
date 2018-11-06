static void dissect_tns_connect(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *connect_tree = NULL, *ti;
	int cd_offset;
	int cd_len;
	int tns_offset = offset-8;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Connect");
		connect_tree = proto_item_add_subtree(ti, ett_tns_connect);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_connect, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Connect");
	}

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_version, tvb,
			offset, 2, FALSE);
	}
	offset += 2;
	
	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_compat_version, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		/
		proto_tree_add_item(connect_tree, hf_tns_service_options, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_sdu_size, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_max_tdu_size, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_nt_proto_characteristics, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_line_turnaround, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_value_of_one, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	cd_len = tvb_get_ntohs(tvb, offset);
	if ( connect_tree )
	{
		proto_tree_add_uint(connect_tree, hf_tns_connect_data_length, tvb,
			offset, 2, cd_len);
	}
	offset += 2;

	cd_offset = tvb_get_ntohs(tvb, offset);
	if ( connect_tree )
	{
		proto_tree_add_uint(connect_tree, hf_tns_connect_data_offset, tvb,
			offset, 2, cd_offset);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_connect_data_max, tvb,
			offset, 4, FALSE);
	}
	offset += 4;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_connect_flags0, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_connect_flags1, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_trace_cf1, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_trace_cf2, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree )
	{
		proto_tree_add_item(connect_tree, hf_tns_trace_cid, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( connect_tree && cd_len > 0)
	{
		proto_tree_add_item(connect_tree, hf_tns_connect_data, tvb,
			tns_offset+cd_offset,
			tvb_length_remaining(tvb, tns_offset+cd_offset), FALSE);
	}
	return;
}
