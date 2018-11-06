static int
dissect_browser_TYPE_12_data(tvbuff_t *tvb, int offset,
			packet_info *pinfo, proto_tree *tree,
			guint8 *drep)
{
	guint32 len;
	dcerpc_info *di;

	di=pinfo->private_data;
	if(di->conformant_run){
		/
		offset =dissect_ndr_ucarray(tvb, offset, pinfo, tree, drep, NULL);

		return offset;
	}

	/
	offset = dissect_ndr_uint32(tvb, offset, pinfo, tree, drep,
		hf_browser_unknown_long, &len);

	proto_tree_add_item(tree, hf_browser_unknown_bytes, tvb, offset, len,
		FALSE);
	offset += len;

	return len;
}
