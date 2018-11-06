guint
proto_mpeg_descriptor_dissect(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint8      tag, len;

	proto_item *di;
	proto_tree *descriptor_tree;

	tag = tvb_get_guint8(tvb, offset);
	len = tvb_get_guint8(tvb, offset + 1);

	di = proto_tree_add_text(tree, tvb, offset, len + 2, "Descriptor Tag=0x%02x", tag);
	descriptor_tree = proto_item_add_subtree(di, ett_mpeg_descriptor);

	proto_tree_add_item(descriptor_tree, hf_mpeg_descriptor_tag,    tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(descriptor_tree, hf_mpeg_descriptor_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (len == 0)
		return 2;

	switch (tag) {
		case 0x02: /
			proto_mpeg_descriptor_dissect_video_stream(tvb, offset, descriptor_tree);
			break;
		case 0x03: /
			proto_mpeg_descriptor_dissect_audio_stream(tvb, offset, descriptor_tree);
			break;
		case 0x06: /
			proto_mpeg_descriptor_dissect_data_stream_alignment(tvb, offset, descriptor_tree);
			break;
		case 0x09: /
			proto_mpeg_descriptor_dissect_ca(tvb, offset, len, descriptor_tree);
			break;
		case 0x0A: /
			proto_mpeg_descriptor_dissect_iso639(tvb, offset, len, descriptor_tree);
			break;
		case 0x0B: /
			proto_mpeg_descriptor_dissect_system_clock(tvb, offset, descriptor_tree);
			break;
		case 0x0E: /
			proto_mpeg_descriptor_dissect_max_bitrate(tvb, offset, descriptor_tree);
			break;
		case 0x10: /
			proto_mpeg_descriptor_dissect_smoothing_buffer(tvb, offset, descriptor_tree);
			break;
		case 0x11: /
			proto_mpeg_descriptor_dissect_std(tvb, offset, descriptor_tree);
			break;
		case 0x13: /
			proto_mpeg_descriptor_dissect_carousel_identifier(tvb, offset, len, descriptor_tree);
			break;
		case 0x14: /
			proto_mpeg_descriptor_dissect_association_tag(tvb, offset, len, descriptor_tree);
			break;
		case 0x28: /
			proto_mpeg_descriptor_dissect_avc_vid(tvb, offset, descriptor_tree);
			break;
		case 0x40: /
			proto_mpeg_descriptor_dissect_network_name(tvb, offset, len, descriptor_tree);
			break;
		case 0x41: /
			proto_mpeg_descriptor_dissect_service_list(tvb, offset, len, descriptor_tree);
			break;
		case 0x42: /
			proto_mpeg_descriptor_stuffing(tvb, offset, len, descriptor_tree);
			break;
		case 0x43: /
			proto_mpeg_descriptor_dissect_satellite_delivery(tvb, offset, descriptor_tree);
			break;
		case 0x44: /
			proto_mpeg_descriptor_dissect_cable_delivery(tvb, offset, descriptor_tree);
			break;
		case 0x45: /
			proto_mpeg_descriptor_dissect_vbi_data(tvb, offset, len, descriptor_tree);
			break;
		case 0x47: /
			proto_mpeg_descriptor_dissect_bouquet_name(tvb, offset, len, descriptor_tree);
			break;
		case 0x48: /
			proto_mpeg_descriptor_dissect_service(tvb, offset, descriptor_tree);
			break;
		case 0x4A: /
			proto_mpeg_descriptor_dissect_linkage(tvb, offset, len, descriptor_tree);
			break;
		case 0x4D: /
			proto_mpeg_descriptor_dissect_short_event(tvb, offset, descriptor_tree);
			break;
		case 0x4E: /
			proto_mpeg_descriptor_dissect_extended_event(tvb, offset, descriptor_tree);
			break;
		case 0x50: /
			proto_mpeg_descriptor_dissect_component(tvb, offset, len, descriptor_tree);
			break;
		case 0x52: /
			proto_mpeg_descriptor_dissect_stream_identifier(tvb, offset, descriptor_tree);
			break;
		case 0x53: /
			proto_mpeg_descriptor_dissect_ca_identifier(tvb, offset, len, descriptor_tree);
			break;
		case 0x54: /
			proto_mpeg_descriptor_dissect_content(tvb, offset, len, descriptor_tree);
			break;
		case 0x55: /
			proto_mpeg_descriptor_dissect_parental_rating(tvb, offset, descriptor_tree);
			break;
		case 0x56: /
			proto_mpeg_descriptor_dissect_teletext(tvb, offset, len, descriptor_tree);
			break;
		case 0x58: /
			proto_mpeg_descriptor_dissect_local_time_offset(tvb, offset, len, descriptor_tree);
			break;
		case 0x59: /
			proto_mpeg_descriptor_dissect_subtitling(tvb, offset, len, descriptor_tree);
			break;
		case 0x5A: /
			proto_mpeg_descriptor_dissect_terrestrial_delivery(tvb, offset, descriptor_tree);
			break;
		case 0x5F: /
			proto_mpeg_descriptor_dissect_private_data_specifier(tvb, offset, descriptor_tree);
			break;
		case 0x64: /
			proto_mpeg_descriptor_dissect_data_bcast(tvb, offset, descriptor_tree);
			break;
		case 0x66: /
			proto_mpeg_descriptor_dissect_data_bcast_id(tvb, offset, len, descriptor_tree);
			break;
		case 0x6A: /
			proto_mpeg_descriptor_dissect_ac3(tvb, offset, len, descriptor_tree);
			break;
		case 0x6F: /
			proto_mpeg_descriptor_dissect_app_sig(tvb, offset, len, descriptor_tree);
			break;
		case 0x73: /
			proto_mpeg_descriptor_dissect_default_authority(tvb, offset, len, descriptor_tree);
			break;
		case 0x76: /
			proto_mpeg_descriptor_dissect_content_identifier(tvb, offset, len, descriptor_tree);
			break;
		case 0x7F: /
			proto_mpeg_descriptor_dissect_extension(tvb, offset, len, descriptor_tree);
			break;
		case 0xA2: /
			proto_mpeg_descriptor_dissect_logon_initialize(tvb, offset, len, descriptor_tree);
			break;
		case 0xA7: /
			proto_mpeg_descriptor_dissect_rcs_content(tvb, offset, len, descriptor_tree);
			break;
		default:
			proto_tree_add_item(descriptor_tree, hf_mpeg_descriptor_data, tvb, offset, len, ENC_NA);
			break;
	}

	return len + 2;
}
