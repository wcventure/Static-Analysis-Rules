static int
dissect_spoolss_keybuffer(tvbuff_t *tvb, int offset, packet_info *pinfo,
			  proto_tree *tree, char *drep)
{
	dcerpc_info *di = pinfo->private_data;
	guint32 size, key_start;

	if (di->conformant_run)
		return offset;

	/

	offset = dissect_ndr_uint32(tvb, offset, pinfo, tree, drep,
				    hf_spoolss_keybuffer_size, &size);

	key_start = offset;
	while (key_start + (size*2) > offset) {
		offset = prs_uint16uni(tvb, offset, pinfo, tree,
				       NULL, "Key");
	}

	return offset;
}
