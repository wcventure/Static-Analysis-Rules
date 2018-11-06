static int dissect_PRINTER_INFO_3(tvbuff_t *tvb, int offset, 
				  packet_info *pinfo, proto_tree *tree, 
				  guint8 *drep)
{
	offset = dissect_ndr_uint32(
		tvb, offset, pinfo, tree, drep,
		hf_printer_flags, NULL);
	
	offset = dissect_nt_sec_desc(
		tvb, offset, pinfo, tree, drep, 
		tvb_length_remaining(tvb, offset), 
		&spoolss_printer_access_mask_info);

	return offset;
}
