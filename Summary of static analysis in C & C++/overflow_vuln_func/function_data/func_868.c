static void
dissect_isdn(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_tree *isdn_tree;
	proto_item *ti;
	static const char v120_sabme[3] = { 0x08, 0x01, 0x7F };
	static const char ppp[2] = { 0xFF, 0x03 };
	circuit_t *circuit;

	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "ISDN");

	if (pinfo->pseudo_header->isdn.uton) {
		if (check_col(pinfo->cinfo, COL_RES_DL_DST))
			col_set_str(pinfo->cinfo, COL_RES_DL_DST, "Network");
		if (check_col(pinfo->cinfo, COL_RES_DL_SRC))
			col_set_str(pinfo->cinfo, COL_RES_DL_SRC, "User");
	} else {
		if (check_col(pinfo->cinfo, COL_RES_DL_DST))
		    col_set_str(pinfo->cinfo, COL_RES_DL_DST, "User");
		if (check_col(pinfo->cinfo, COL_RES_DL_SRC))
		    col_set_str(pinfo->cinfo, COL_RES_DL_SRC, "Network");
	}

	pinfo->ctype = CT_ISDN;
	pinfo->circuit_id = pinfo->pseudo_header->isdn.channel;

	if (tree) {
		ti = proto_tree_add_item(tree, proto_isdn, tvb, 0, 0, FALSE);
		isdn_tree = proto_item_add_subtree(ti, ett_isdn);

		proto_tree_add_uint(isdn_tree, hf_isdn_channel, tvb, 0, 0,
		    pinfo->pseudo_header->isdn.channel);
	}

	/
	circuit = find_circuit(pinfo->ctype, pinfo->circuit_id, pinfo->fd->num);
	if (circuit == NULL)
		circuit = circuit_new(pinfo->ctype, pinfo->circuit_id,
		    pinfo->fd->num);

	if (circuit_get_dissector(circuit) == NULL) {
		/
		switch (pinfo->pseudo_header->isdn.channel) {

		case 0:
			/
			circuit_set_dissector(circuit, lapd_handle);
			break;

		default:
			/
			if (tvb_memeql(tvb, 0, v120_sabme, 3) == 0) {
				/
				circuit_set_dissector(circuit, v120_handle);
			} else if (tvb_memeql(tvb, 0, ppp, 2) == 0) {
				/
				circuit_set_dissector(circuit, ppp_hdlc_handle);
			}
			break;
		}
	}

	if (!try_circuit_dissector(pinfo->ctype, pinfo->circuit_id,
	    pinfo->fd->num, tvb, pinfo, tree))
		call_dissector(data_handle, tvb, pinfo, tree);
}
