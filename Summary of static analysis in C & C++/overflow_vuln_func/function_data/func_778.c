static int
dissect_SEC_DESC_BUF(tvbuff_t *tvb, int offset, packet_info *pinfo,
		     proto_tree *tree, guint8 *drep)
{
	proto_item *item;
	proto_tree *subtree;
	guint32 len;

	/

	item = proto_tree_add_text(
		tree, tvb, offset, 0, "Security descriptor buffer");

	subtree = proto_item_add_subtree(item, ett_SEC_DESC_BUF);

        offset = dissect_ndr_uint32(
                tvb, offset, pinfo, subtree, drep,
                hf_secdescbuf_maxlen, NULL);

        offset = dissect_ndr_uint32(
                tvb, offset, pinfo, subtree, drep,
                hf_secdescbuf_undoc, NULL);

        offset = dissect_ndr_uint32(
                tvb, offset, pinfo, subtree, drep,
                hf_secdescbuf_len, &len);
	
	dissect_nt_sec_desc(
		tvb, offset, pinfo, subtree, drep, len, 
		&spoolss_printer_access_mask_info);

	offset += len;	

	return offset;
}
