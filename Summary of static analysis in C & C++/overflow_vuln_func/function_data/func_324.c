static int
dissect_nfs4_open_read_delegation(tvbuff_t *tvb, int offset,
			packet_info *pinfo, proto_tree *tree)
{
	offset = dissect_nfs4_stateid(tvb, offset, tree, NULL);
	offset = dissect_rpc_bool(tvb, tree, hf_nfs4_recall4, offset);
	offset = dissect_nfs4_ace(tvb, offset, pinfo, tree);

	return offset;
}
