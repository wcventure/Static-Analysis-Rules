static int
dissect_nfs4_open_write_delegation(tvbuff_t *tvb, int offset,
	packet_info *pinfo, proto_tree *tree)
{
	offset = dissect_nfs4_stateid(tvb, offset, tree, NULL);
	offset = dissect_rpc_bool(tvb, tree, hf_nfs4_recall, offset);
	offset = dissect_nfs4_space_limit(tvb, offset, tree);
	offset = dissect_nfs4_ace(tvb, offset, pinfo, tree);

	return offset;
}
