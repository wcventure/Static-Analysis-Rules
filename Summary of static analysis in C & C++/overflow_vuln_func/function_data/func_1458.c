static void
proto_mpeg_descriptor_dissect_extension(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint     offset_start;
	guint8    tag_ext;
	gboolean  lang_code_present;
	guint     already_dissected;

	offset_start = offset;

	tag_ext = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_extension_tag_extension, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	switch (tag_ext) {
		case EXT_TAG_SUPP_AUDIO:
			proto_tree_add_item(tree, hf_mpeg_descr_extension_supp_audio_mix_type, tvb, offset, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(tree, hf_mpeg_descr_extension_supp_audio_ed_cla, tvb, offset, 1, ENC_BIG_ENDIAN);
			lang_code_present = ((tvb_get_guint8(tvb, offset) & 0x01) == 0x01);
			proto_tree_add_item(tree, hf_mpeg_descr_extension_supp_audio_lang_code_present, tvb, offset, 1, ENC_BIG_ENDIAN);
			offset++;
			if (lang_code_present) {
				proto_tree_add_item(tree, hf_mpeg_descr_extension_supp_audio_lang_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
				offset += 3;
			}
			already_dissected = offset-offset_start;
			if (already_dissected<len)
				proto_tree_add_text(tree, tvb, offset, len-already_dissected, "Private data");
			break;
		default:
			already_dissected = offset-offset_start;
			if (already_dissected<len)
				proto_tree_add_item(tree, hf_mpeg_descr_extension_data, tvb, offset, len-already_dissected, ENC_NA);
			break;
	}

}
