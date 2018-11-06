static int
dissect_nfs4_compound_reply(tvbuff_t *tvb, int offset, packet_info *pinfo,
	proto_tree* tree)
{
	guint32 status;
	const char *tag=NULL;

	offset = dissect_nfs4_status(tvb, offset, tree, &status);
	offset = dissect_nfs_utf8string(tvb, offset, tree, hf_nfs4_tag, &tag);
	/
	if (nfs_display_v4_tag && strncmp(tag,"<EMPTY>",7) != 0) 
		col_append_fstr(pinfo->cinfo, COL_INFO," %s", tag);
	
	offset = dissect_nfs4_response_op(tvb, offset, pinfo, tree);

	if (status != NFS4_OK)
		col_append_fstr(pinfo->cinfo, COL_INFO," Status: %s",
				val_to_str_ext(status, &names_nfs4_status_ext, "Unknown error: %u"));

	return offset;
}
