static void
proto_mpeg_descriptor_dissect_terrestrial_delivery(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint32 centre_freq;

	centre_freq = tvb_get_ntohl(tvb, offset);
	proto_tree_add_string_format_value(tree, hf_mpeg_descr_terrestrial_delivery_centre_frequency, tvb, offset, 4,
		"CentreFrequency", "%u0 Hz", centre_freq);
	offset += 4;

	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_bandwidth, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_priority, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_time_slicing_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_mpe_fec_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_constellation, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_hierarchy_information, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_code_rate_hp_stream, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_code_rate_lp_stream, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_guard_interval, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_transmission_mode, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_other_frequency_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_terrestrial_delivery_reserved2, tvb, offset, 4, ENC_BIG_ENDIAN);

}
