static void dissect_tns_refuse(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree *tree, proto_tree *tns_tree)
{
	proto_tree *refuse_tree = NULL, *ti;

	if ( tree )
	{
		ti = proto_tree_add_text(tns_tree, tvb, offset, -1,
		    "Refuse");
		refuse_tree = proto_item_add_subtree(ti, ett_tns_refuse);

		proto_tree_add_boolean_hidden(tns_tree, hf_tns_refuse, tvb,
		    0, 0, TRUE);
	}
		
	if ( check_col(pinfo->cinfo, COL_INFO) )
	{
		col_append_str(pinfo->cinfo, COL_INFO, ", Refuse");
	}

	if ( refuse_tree )
	{
		proto_tree_add_item(refuse_tree, hf_tns_refuse_reason_user, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( refuse_tree )
	{
		proto_tree_add_item(refuse_tree, hf_tns_refuse_reason_system, tvb,
			offset, 1, FALSE);
	}
	offset += 1;

	if ( refuse_tree )
	{
		proto_tree_add_item(refuse_tree, hf_tns_refuse_data_length, tvb,
			offset, 2, FALSE);
	}
	offset += 2;

	if ( refuse_tree )
	{
		proto_tree_add_item(refuse_tree, hf_tns_refuse_data, tvb,
			offset, tvb_length_remaining(tvb, offset), FALSE);
	}
	return;
}
