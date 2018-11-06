int
dissect_dcom_BSTR(tvbuff_t *tvb, gint offset, packet_info *pinfo,
		       proto_tree *tree, dcerpc_info *di, guint8 *drep, int hfindex,
					   gchar *pszStr, guint32 u32MaxStr)
{
	guint32 u32MaxCount;
	guint32 u32ArraySize;
	guint32 u32StrStart;
	proto_item *sub_item;
	proto_tree *sub_tree;
	guint32 u32SubStart;
	guint32 u32ByteLength;
	guint32	u32RealOffset;
	gboolean isPrintable;

	/
	if (offset % 4) {
		offset += 4 - (offset % 4);
	}

	/
	sub_item = proto_tree_add_string(tree, hfindex, tvb, offset, 0, "");
	sub_tree = proto_item_add_subtree(sub_item, ett_dcom_lpwstr);
	u32SubStart = offset;

	offset = dissect_dcom_DWORD(tvb, offset, pinfo, sub_tree, di, drep,
			hf_dcom_max_count, &u32MaxCount);
	offset = dissect_dcom_DWORD(tvb, offset, pinfo, sub_tree, di, drep,
			hf_dcom_byte_length, &u32ByteLength);
	offset = dissect_dcom_dcerpc_array_size(tvb, offset, pinfo, sub_tree, di, drep,
			&u32ArraySize);

	u32RealOffset = offset + u32ArraySize*2;

	u32StrStart = offset;
	offset = dcom_tvb_get_nwstringz0(tvb, offset, u32ArraySize*2, pszStr, u32MaxStr, &isPrintable);

	proto_tree_add_string(sub_tree, hfindex, tvb, u32StrStart, offset - u32StrStart, pszStr);

	/
	proto_item_append_text(sub_item, "%s%s%s",
	isPrintable ? "\"" : "", pszStr, isPrintable ? "\"" : "");
	if ((int) (u32RealOffset - u32SubStart) <= 0)
		THROW(ReportedBoundsError);
	proto_item_set_len(sub_item, u32RealOffset - u32SubStart);

	return u32RealOffset;
}
