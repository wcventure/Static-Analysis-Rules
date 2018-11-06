static int
dissect_nsh(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{

	int offset = 0;
	int md_type = -1;
	int nsh_bytes_len = 0;
	int nsh_next_proto = -1;
	int captured_length;
	tvbuff_t *next_tvb;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "NSH");
	col_set_str(pinfo->cinfo, COL_INFO, "Network Service Header");

	captured_length = tvb_captured_length(tvb);


	if (tree) {
		proto_item *ti;
		proto_tree *nsh_tree;

		/
		nsh_bytes_len = 4 * tvb_get_bits8(tvb, 10, 6);

		ti = proto_tree_add_item(tree, proto_nsh, tvb, offset, nsh_bytes_len, ENC_NA);
		nsh_tree = proto_item_add_subtree(ti, ett_nsh);

		/

		proto_tree_add_item(nsh_tree, hf_nsh_version, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(nsh_tree, hf_nsh_oam, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(nsh_tree, hf_nsh_critical_metadata, tvb, offset, 2, ENC_BIG_ENDIAN);


		/
		proto_tree_add_item(nsh_tree, hf_nsh_reservedbits, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(nsh_tree, hf_nsh_length, tvb, offset, 2, ENC_BIG_ENDIAN);


		md_type = tvb_get_guint8(tvb, offset + 2);
		proto_tree_add_item(nsh_tree, hf_nsh_md_type, tvb, offset + 2, 1, ENC_BIG_ENDIAN);

		nsh_next_proto = tvb_get_guint8(tvb, offset + 3);
		proto_tree_add_item(nsh_tree, hf_nsh_next_proto, tvb, offset + 3, 1, ENC_BIG_ENDIAN);

		/
		offset = offset + 4;
		proto_tree_add_item(nsh_tree, hf_nsh_service_pathID, tvb, offset, 3, ENC_BIG_ENDIAN);
		proto_tree_add_item(nsh_tree, hf_nsh_service_index, tvb, offset + 3, 1, ENC_BIG_ENDIAN);

		/
		offset = offset + 4;
		switch (md_type) {

		case MD_TYPE_1:
			dissect_nsh_md_type_1(tvb, nsh_tree, offset);
			break;

		case MD_TYPE_2:

			/
			if (nsh_bytes_len > 8)
				dissect_nsh_md_type_2(tvb, nsh_tree, offset, nsh_bytes_len);
			break;

		}

		/

		if (captured_length > (nsh_bytes_len)) {

			next_tvb = tvb_new_subset_remaining(tvb, nsh_bytes_len);
			if (!dissector_try_uint(subdissector_table, nsh_next_proto, tvb, pinfo, tree)) {
				call_data_dissector(next_tvb, pinfo, tree);
			}
		}
	}

	return tvb_captured_length(tvb);

}
