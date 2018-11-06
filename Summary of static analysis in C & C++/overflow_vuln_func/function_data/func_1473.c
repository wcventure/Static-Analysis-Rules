static void
dissect_dsmcc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "DSM-CC");
	dissect_dsmcc_ts(tvb, pinfo, tree, NULL);
}
