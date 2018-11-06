static void dissect_gnutella(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) {

	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "Gnutella");

	if (check_col(pinfo->cinfo, COL_INFO))
		col_clear(pinfo->cinfo, COL_INFO);

	tcp_dissect_pdus(tvb, pinfo, tree, TRUE, 2, get_gnutella_pdu_len,
	    dissect_gnutella_pdu);
}
