static int
dissect_nfs4_compound_call(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree* tree)
{
	const char *tag=NULL;

	offset = dissect_nfs_utf8string(tvb, offset, tree, hf_nfs4_tag, &tag);
	/
	if (nfs_display_v4_tag && strncmp(tag,"<EMPTY>",7)!=0) 
		col_append_fstr(pinfo->cinfo, COL_INFO," %s", tag);

	offset = dissect_rpc_uint32(tvb, tree, hf_nfs4_minorversion, offset);
	offset = dissect_nfs4_request_op(tvb, offset, pinfo, tree);

	return offset;
}
