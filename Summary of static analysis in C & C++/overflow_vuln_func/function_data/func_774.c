int
lsa_dissect_sec_desc_buf_data(tvbuff_t *tvb, int offset,
                             packet_info *pinfo, proto_tree *tree,
                             guint8 *drep)
{
	guint32 len;
	dcerpc_info *di;

	di=pinfo->private_data;
	if(di->conformant_run){
		/
		return offset;
	}

	offset = dissect_ndr_uint32 (tvb, offset, pinfo, tree, drep,
				     hf_lsa_sd_size, &len);

	tvb_ensure_bytes_exist(tvb, offset, len);
	dissect_nt_sec_desc(
		tvb, offset, pinfo, tree, drep, len, &lsa_access_mask_info);

	offset += len;

	return offset;
}
