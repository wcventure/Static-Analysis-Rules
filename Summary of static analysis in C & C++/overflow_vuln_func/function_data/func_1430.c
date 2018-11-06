static void
proto_mpeg_descriptor_dissect_avc_vid(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_profile_idc, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_constraint_set0_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_constraint_set1_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_constraint_set2_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_compatible_flags, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_level_idc, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_still_present, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_24h_picture_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_avc_vid_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
}
