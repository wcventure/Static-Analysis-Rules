static guint
proto_mpeg_descriptor_dissect_private_ciplus(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint        offset_start;
	guint8       tag, len;
	const gchar *tag_str;
	proto_item  *di;
	proto_tree  *descriptor_tree;
	guint8       str_len_byte;

	offset_start=offset;

	tag = tvb_get_guint8(tvb, offset);
	tag_str = match_strval(tag, mpeg_descriptor_ciplus_tag_vals);
	if (!tag_str)
		return 0;

	di = proto_tree_add_text(tree, tvb, offset_start, -1, "CI+ private descriptor Tag=0x%02x", tag);
	descriptor_tree = proto_item_add_subtree(di, ett_mpeg_descriptor);

	proto_tree_add_uint_format(descriptor_tree, hf_mpeg_descriptor_tag, tvb, offset, 1, tag, "Descriptor Tag: %s (0x%x)", tag_str, tag);
	offset++;

	len = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(descriptor_tree, hf_mpeg_descriptor_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (tag==0xCB) {
		proto_tree_add_item(tree, hf_mpeg_descr_ciplus_cl_cb_min, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_ciplus_cl_cb_max, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_ciplus_cl_lang, tvb, offset, 3, ENC_BIG_ENDIAN);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_ciplus_cl_label, tvb, offset, len-offset, ENC_BIG_ENDIAN);
		offset += len-offset;
	}
	else if (tag==0xCC) {
		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_visible, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_selectable, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_lcn, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		str_len_byte = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_prov_name, tvb, offset, 1, ENC_ASCII|ENC_NA);
		offset += 1+str_len_byte;

		str_len_byte = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(descriptor_tree, hf_mpeg_descr_ciplus_svc_name, tvb, offset, 1, ENC_ASCII|ENC_NA);
		offset += 1+str_len_byte;
	}

	proto_item_set_len(di, offset-offset_start);
	return offset-offset_start;
}
