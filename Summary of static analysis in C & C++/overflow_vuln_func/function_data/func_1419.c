static void
proto_mpeg_descriptor_dissect_video_stream(tvbuff_t *tvb, guint offset, proto_tree *tree)
{

	guint8 mpeg1_only_flag;

	mpeg1_only_flag = tvb_get_guint8(tvb, offset) & MPEG_DESCR_VIDEO_STREAM_MPEG1_ONLY_FLAG_MASK;
	proto_tree_add_item(tree, hf_mpeg_descr_video_stream_multiple_frame_rate_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_video_stream_frame_rate_code, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_video_stream_mpeg1_only_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_video_stream_constrained_parameter_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_video_stream_still_picture_flag, tvb, offset, 1, ENC_BIG_ENDIAN);

	offset++;

	if (mpeg1_only_flag == 0) {

		proto_tree_add_item(tree, hf_mpeg_descr_video_stream_profile_and_level_indication, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(tree, hf_mpeg_descr_video_stream_chroma_format, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_video_stream_frame_rate_extension_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_video_stream_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
	}
}
