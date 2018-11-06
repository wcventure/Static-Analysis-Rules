static int
dissect_nfs4_fattr_acl(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree)
{
	return dissect_rpc_array(tvb, pinfo, tree, offset, dissect_nfs4_ace,
		hf_nfs4_acl);
}
