void
proto_register_mpeg_descriptor(void)
{

	static hf_register_info hf[] = {
		{ &hf_mpeg_descriptor_tag, {
			"Descriptor Tag", "mpeg_descr.tag",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descriptor_tag_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descriptor_length, {
			"Descriptor Length", "mpeg_descr.len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descriptor_data, {
			"Descriptor Data", "mpeg_descr.data",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_video_stream_multiple_frame_rate_flag, {
			"Multiple Frame Rate Flag", "mpeg_descr.video_stream.multiple_frame_rate_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_video_stream_multiple_frame_rate_flag_vals),
			MPEG_DESCR_VIDEO_STREAM_MULTIPLE_FRAME_RATE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_frame_rate_code, {
			"Frame Rate Code", "mpeg_descr.video_stream.frame_rate_code",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_VIDEO_STREAM_FRAME_RATE_CODE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_mpeg1_only_flag, {
			"MPEG1 Only Flag", "mpeg_descr.video_stream.mpeg1_only_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_VIDEO_STREAM_MPEG1_ONLY_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_constrained_parameter_flag, {
			"Constrained Parameter Flag", "mpeg_descr.video_stream.constrained_parameter_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_VIDEO_STREAM_CONSTRAINED_PARAMETER_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_still_picture_flag, {
			"Still Picture Flag", "mpeg_descr.video_stream.still_picture_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_VIDEO_STREAM_STILL_PICTURE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_profile_and_level_indication, {
			"Profile and Level Indication", "mpeg_descr.video_stream.profile_level_ind",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_chroma_format, {
			"Chroma Format", "mpeg_descr.video_stream.chroma_format",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_frame_rate_extension_flag, {
			"Frame Rate Extension Flag", "mpeg_descr.video_stream.frame_rate_extension_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_VIDEO_STREAM_FRAME_RATE_EXTENSION_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_video_stream_reserved, {
			"Reserved", "mpeg_descr.video_stream.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_VIDEO_STREAM_RESERVED_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_audio_stream_free_format_flag, {
			"Free Format Flag", "mpeg_descr.audio_stream.free_format_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_audio_stream_free_format_flag_vals), MPEG_DESCR_AUDIO_STREAM_FREE_FORMAT_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_audio_stream_id, {
			"ID", "mpeg_descr.audio_stream.id",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_audio_stream_id_vals), MPEG_DESCR_AUDIO_STREAM_ID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_audio_stream_layer, {
			"Layer", "mpeg_descr.audio_stream.layer",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AUDIO_STREAM_LAYER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_audio_stream_variable_rate_audio_indicator, {
			"Variable Rate Audio Indicator", "mpeg_descr.audio_stream.vbr_indicator",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_audio_stream_variable_rate_audio_indicator_vals),
			MPEG_DESCR_AUDIO_STREAM_VARIABLE_RATE_AUDIO_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_audio_stream_reserved, {
			"Reserved", "mpeg_descr.audio_stream.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_AUDIO_STREAM_RESERVED_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_data_stream_alignment, {
			"Data Stream Alignment", "mpeg_descr.data_stream_alignment.alignment",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_data_stream_alignment_vals), 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_ca_system_id, {
			"System ID", "mpeg_descr.ca.sys_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ca_reserved, {
			"Reserved", "mpeg_descr.ca.reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_CA_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ca_pid, {
			"CA PID", "mpeg_descr.ca.pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_CA_PID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ca_private, {
			"Private bytes", "mpeg_descr.ca.private",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_iso639_lang, {
			"ISO 639 Language Code", "mpeg_descr.lang.code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_iso639_type, {
			"ISO 639 Language Type", "mpeg_descr.lang.type",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_iso639_type_vals), 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_system_clock_external_clock_reference_indicator, {
			"External Clock Reference Indicator", "mpeg_descr.sys_clk.external_clk_ref_ind",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_SYSTEM_CLOCK_EXTERNAL_CLOCK_REFERENCE_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_system_clock_reserved1, {
			"Reserved", "mpeg_descr.sys_clk.reserved1",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_SYSTEM_CLOCK_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_system_clock_accuracy_integer, {
			"Accuracy Integer", "mpeg_descr.sys_clk.accuracy_integer",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_SYSTEM_CLOCK_ACCURACY_INTEGER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_system_clock_accuracy_exponent, {
			"Accuracy Exponent", "mpeg_descr.sys_clk.accuracy_exponent",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_SYSTEM_CLOCK_ACCURACY_EXPONENT_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_system_clock_reserved2, {
			"Reserved", "mpeg_descr.sys_clk.reserved2",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_SYSTEM_CLOCK_RESERVED2_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_max_bitrate_reserved, {
			"Maximum Bitrate Reserved", "mpeg_descr.max_bitrate.reserved",
			FT_UINT24, BASE_HEX, NULL, MPEG_DESCR_MAX_BITRATE_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_max_bitrate, {
			"Maximum Bitrate", "mpeg_descr.max_bitrate.rate",
			FT_UINT24, BASE_DEC, NULL, MPEG_DESCR_MAX_BITRATE_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_smoothing_buffer_reserved1, {
			"Reserved", "mpeg_descr.smoothing_buf.reserved1",
			FT_UINT24, BASE_HEX, NULL, MPEG_DESCR_SMOOTHING_BUFFER_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_smoothing_buffer_leak_rate, {
			"Leak Rate", "mpeg_descr.smoothing_buf.leak_rate",
			FT_UINT24, BASE_DEC, NULL, MPEG_DESCR_SMOOTHING_BUFFER_LEAK_RATE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_smoothing_buffer_reserved2, {
			"Reserved", "mpeg_descr.smoothing_buf.reserved2",
			FT_UINT24, BASE_HEX, NULL, MPEG_DESCR_SMOOTHING_BUFFER_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_smoothing_buffer_size, {
			"Buffer Size", "mpeg_descr.smoothing_buf.size",
			FT_UINT24, BASE_DEC, NULL, MPEG_DESCR_SMOOTHING_BUFFER_SIZE_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_std_reserved, {
			"Reserved", "mpeg_descr.std.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_STD_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_std_leak_valid, {
			"Leak Valid", "mpeg_descr.std.leak_valid",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_STD_LEAK_VALID_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_carousel_identifier_id, {
			"Carousel ID", "mpeg_descr.carousel_identifier.id",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_format_id, {
			"Format ID", "mpeg_descr.carousel_identifier.format_id",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_carousel_identifier_format_id_vals), 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_module_version, {
			"Module Version", "mpeg_descr.carousel_identifier.module_version",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_module_id, {
			"Module ID", "mpeg_descr.carousel_identifier.module_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_block_size, {
			"Block Size", "mpeg_descr.carousel_identifier.block_size",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_module_size, {
			"Module Size", "mpeg_descr.carousel_identifier.module_size",
			FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_compression_method, {
			"Compression Method", "mpeg_descr.carousel_identifier.comp_method",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_original_size, {
			"Original Size", "mpeg_descr.carousel_identifier.orig_size",
			FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_timeout, {
			"Timeout", "mpeg_descr.carousel_identifier.timeout",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_object_key_len, {
			"Object Key Length", "mpeg_descr.carousel_identifier.key_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_object_key_data, {
			"Object Key Data", "mpeg_descr.carousel_identifier.key_data",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_carousel_identifier_private, {
			"Private Bytes", "mpeg_descr.carousel_identifier.private",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_association_tag, {
			"Association Tag", "mpeg_descr.assoc_tag.tag",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_use, {
			"Use", "mpeg_descr.assoc_tag.use",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_selector_len, {
			"Selector Length", "mpeg_descr.assoc_tag.selector_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_transaction_id, {
			"Transaction ID", "mpeg_descr.assoc_tag.transaction_id",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_timeout, {
			"Timeout", "mpeg_descr.assoc_tag.timeout",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_selector_bytes, {
			"Selector Bytes", "mpeg_descr.assoc_tag.selector_bytes",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_association_tag_private_bytes, {
			"Private Bytes", "mpeg_descr.assoc_tag.private_bytes",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_avc_vid_profile_idc, {
			"Profile IDC", "mpeg_descr.avc_vid.profile_idc",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_constraint_set0_flag, {
			"Constraint Set0 Flag", "mpeg_descr.avc_vid.contraint_set0",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AVC_VID_CONSTRAINT_SET0_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_constraint_set1_flag, {
			"Constraint Set1 Flag", "mpeg_descr.avc_vid.contraint_set1",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AVC_VID_CONSTRAINT_SET1_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_constraint_set2_flag, {
			"Constraint Set2 Flag", "mpeg_descr.avc_vid.contraint_set2",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AVC_VID_CONSTRAINT_SET2_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_compatible_flags, {
			"Constraint Compatible Flags", "mpeg_descr.avc_vid.compatible_flags",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_AVC_VID_COMPATIBLE_FLAGS_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_level_idc, {
			"Level IDC", "mpeg_descr.avc_vid.level_idc",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_still_present, {
			"AVC Still Present", "mpeg_descr.avc_vid.still_present",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AVC_VID_STILL_PRESENT_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_24h_picture_flag, {
			"AVC 24 Hour Picture Flag", "mpeg_descr.avc_vid.24h_picture_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_AVC_VID_24H_PICTURE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_avc_vid_reserved, {
			"Reserved", "mpeg_descr.avc_vid.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_AVC_VID_RESERVED_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_network_name_descriptor, {
			"Network Name", "mpeg_descr.net_name.name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_service_list_id, {
			"Service ID", "mpeg_descr.svc_list.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_service_list_type, {
			"Service Type", "mpeg_descr.svc_list.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_service_type_vals_ext, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_stuffing, {
			"Stuffing", "mpeg_descr.stuffing",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_satellite_delivery_frequency, {
			"Frequency", "mpeg_descr.sat_delivery.freq",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_orbital_position, {
			"Orbital Position", "mpeg_descr.sat_delivery.orbital_pos",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_west_east_flag, {
			"West East Flag", "mpeg_descr.sat_delivery.west_east_flag",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_satellite_delivery_west_east_flag_vals),
			MPEG_DESCR_SATELLITE_DELIVERY_WEST_EAST_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_polarization, {
			"Polarization", "mpeg_descr.sat_delivery.polarization",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_satellite_delivery_polarization_vals),
			MPEG_DESCR_SATELLITE_DELIVERY_POLARIZATION_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_roll_off, {
			"Roll Off", "mpeg_descr.sat_delivery.roll_off",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_satellite_delivery_roll_off_vals),
			MPEG_DESCR_SATELLITE_DELIVERY_ROLL_OFF_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_zero, {
			"Zero", "mpeg_descr.sat_delivery.zero",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_SATELLITE_DELIVERY_ZERO_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_modulation_system, {
			"Modulation System", "mpeg_descr.sat_delivery.modulation_system",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_satellite_delivery_modulation_system_vals),
			MPEG_DESCR_SATELLITE_DELIVERY_MODULATION_SYSTEM_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_modulation_type, {
			"Modulation Type", "mpeg_descr.sat_delivery.modulation_type",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_satellite_delivery_modulation_type_vals),
			MPEG_DESCR_SATELLITE_DELIVERY_MODULATION_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_symbol_rate, {
			"Symbol Rate", "mpeg_descr.sat_delivery.symbol_rate",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_satellite_delivery_fec_inner, {
			"FEC Inner", "mpeg_descr.sat_delivery.fec_inner",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_satellite_delivery_fec_inner_vals_ext,
			MPEG_DESCR_SATELLITE_DELIVERY_FEC_INNER_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_cable_delivery_frequency, {
			"Frequency", "mpeg_descr.cable_delivery.freq",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_cable_delivery_reserved, {
			"Reserved", "mpeg_descr.cable_delivery.reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_CABLE_DELIVERY_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_cable_delivery_fec_outer, {
			"FEC Outer", "mpeg_descr.cable_delivery.fec_outer",
			FT_UINT16, BASE_HEX, VALS(mpeg_descr_cable_delivery_fec_outer_vals),
			MPEG_DESCR_CABLE_DELIVERY_FEC_OUTER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_cable_delivery_modulation, {
			"Modulation", "mpeg_descr.cable_delivery.modulation",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_cable_delivery_modulation_vals), 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_cable_delivery_symbol_rate, {
			"Symbol Rate", "mpeg_descr.cable_delivery.sym_rate",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_cable_delivery_fec_inner, {
			"FEC Inner", "mpeg_descr.cable_delivery.fec_inner",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_cable_delivery_fec_inner_vals_ext,
			MPEG_DESCR_CABLE_DELIVERY_FEC_INNER_MASK, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_vbi_data_service_id, {
			"Data Service ID", "mpeg_descr.vbi_data.svc_id",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_vbi_data_service_id_vals), 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_vbi_data_descr_len, {
			"Data Descriptor Length", "mpeg_descr.vbi_data.decr_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_vbi_data_reserved1, {
			"Reserved", "mpeg_descr.vbi_data.reserved1",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_VBI_DATA_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_vbi_data_field_parity, {
			"Field Parity", "mpeg_descr.vbi_data.field_parity",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_vbi_data_field_parity_vals),
			MPEG_DESCR_VBI_DATA_FIELD_PARITY_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_vbi_data_line_offset, {
			"Line offset", "mpeg_descr.vbi_data.line_offset",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_vbi_data_reserved2, {
			"Reserved", "mpeg_descr.vbi_data.reserved2",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_bouquet_name, {
			"Bouquet Name Descriptor", "mpeg_descr.bouquet_name.name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_service_type, {
			"Service Type", "mpeg_descr.svc.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_service_type_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_service_provider_name_length, {
			"Provider Name Length", "mpeg_descr.svc.provider_name_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_service_provider, {
			"Service Provider Name", "mpeg_descr.svc.provider_name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_service_name_length, {
			"Service Name Length", "mpeg_descr.svc.svc_name_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_service_name, {
			"Service Name", "mpeg_descr.svc.svc_name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_linkage_transport_stream_id, {
			"Transport Stream ID", "mpeg_descr.linkage.tsid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_original_network_id, {
			"Original Network ID", "mpeg_descr.linkage.original_nid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_service_id, {
			"Service ID", "mpeg_descr.linkage.svc_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_linkage_type, {
			"Linkage Type", "mpeg_descr.linkage.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_linkage_linkage_type_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_hand_over_type, {
			"Hand-Over Type", "mpeg_descr.linkage.hand_over_type",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LINKAGE_HAND_OVER_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_reserved1, {
			"Reserved", "mpeg_descr.linkage.reserved1",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LINKAGE_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_origin_type, {
			"Origin Type", "mpeg_descr.linkage.origin_type",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_linkage_origin_type_vals), 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_network_id, {
			"Network ID", "mpeg_descr.linkage.network_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_initial_service_id, {
			"Initial Service ID", "mpeg_descr.linkage.initial_svc_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_target_event_id, {
			"Target Event ID", "mpeg_descr.linkage.target_evt_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_target_listed, {
			"Target Listed", "mpeg_descr.linkage.target_listed",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_linkage_target_listed_vals),
			MPEG_DESCR_LINKAGE_TARGET_LISTED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_event_simulcast, {
			"Event Simulcast", "mpeg_descr.linkage.evt_simulcast",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_linkage_event_simulcast_vals),
			MPEG_DESCR_LINKAGE_EVENT_SIMULCAST_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_reserved2, {
			"Reserved", "mpeg_descr.linkage.reserved2",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LINKAGE_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_private_data_byte, {
			"Private Data", "mpeg_descr.linkage.private_data",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_interactive_network_id, {
			"Interactive Network ID", "mpeg_descr.interactive_network_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_population_id_loop_count, {
			"Population ID loop count", "mpeg_descr.population_id_loop_count",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_population_id, {
			"Population ID", "mpeg_descr.population_id",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_population_id_base, {
			"Population ID Base", "mpeg_descr.population_id_base",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_linkage_population_id_mask, {
			"Population ID Mask", "mpeg_descr.population_id_mask",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_short_event_lang_code, {
			"Language Code", "mpeg_descr.short_evt.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_short_event_name_length, {
			"Event Name Length", "mpeg_descr.short_evt.name_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_short_event_name, {
			"Event Name", "mpeg_descr.short_evt.name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_short_event_text_length, {
			"Event Text Length", "mpeg_descr.short_evt.txt_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_short_event_text, {
			"Event Text", "mpeg_descr.short_evt.txt",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_extended_event_descriptor_number, {
			"Descriptor Number", "mpeg_descr.ext_evt.descr_num",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_EXTENDED_EVENT_DESCRIPTOR_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_last_descriptor_number, {
			"Last Descriptor Number", "mpeg_descr.ext_evt.last_descr_num",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_EXTENDED_EVENT_LAST_DESCRIPTOR_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_lang_code, {
			"Language Code", "mpeg_descr.ext_evt.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_length_of_items, {
			"Length of items", "mpeg_descr.ext_evt.items_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_item_description_length, {
			"Item Description Length", "mpeg_descr.ext_evt.item_descr_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_item_description_char, {
			"Item Description", "mpeg_descr.ext_evt.item_descr",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_item_length, {
			"Item Length", "mpeg_descr.ext_evt.item_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_item_char, {
			"Item", "mpeg_descr.ext_evt.item",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_text_length, {
			"Text Length", "mpeg_descr.ext_evt.txt_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extended_event_text_char, {
			"Text", "mpeg_descr.ext_evt.txt",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_component_reserved, {
			"Reserved", "mpeg_descr.component.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_COMPONENT_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_stream_content, {
			"Stream Content", "mpeg_descr.component.stream_content",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_component_stream_content_vals),
			MPEG_DESCR_COMPONENT_STREAM_CONTENT_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_type, {
			"Component Type", "mpeg_descr.component.type",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_content_type, {
			"Stream Content and Component Type", "mpeg_descr.component.content_type",
			FT_UINT16, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_component_content_type_vals_ext,
			MPEG_DESCR_COMPONENT_CONTENT_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_tag, {
			"Component Tag", "mpeg_descr.component.tag",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_lang_code, {
			"Language Code", "mpeg_descr.component.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_component_text, {
			"Text", "mpeg_descr.component.text",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_stream_identifier_component_tag, {
			"Component Tag", "mpeg_descr.stream_id.component_tag",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_ca_identifier_system_id, {
			"CA System ID", "mpeg_descr.ca_id.sys_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_content_nibble, {
			"Nibble Level 1 and 2", "mpeg_descr.content.nibble_1_2",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_content_nibble_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_nibble_level_1, {
			"Nibble Level 1", "mpeg_descr.content.nibble_lvl_1",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_content_nibble_level_1_vals_ext,
			MPEG_DESCR_CONTENT_NIBBLE_LEVEL_1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_nibble_level_2, {
			"Nibble Level 2", "mpeg_descr.content.nibble_lvl_2",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_CONTENT_NIBBLE_LEVEL_2_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_user_byte, {
			"User Byte", "mpeg_descr.content.user",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_teletext_lang_code, {
			"Language Code", "mpeg_descr.teletext.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_teletext_type, {
			"Teletext Type", "mpeg_descr.teletext.type",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_teletext_type_vals),
			MPEG_DESCR_TELETEXT_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_teletext_magazine_number, {
			"Magazine Number", "mpeg_descr.teletext.magazine_num",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_TELETEXT_MAGAZINE_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_teletext_page_number, {
			"Page Number", "mpeg_descr.teletext.page_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_parental_rating_country_code, {
			"Country Code", "mpeg_descr.parental_rating.country_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_parental_rating_rating, {
			"Rating", "mpeg_descr.parental_rating.rating",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_parental_rating_vals_ext, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_local_time_offset_country_code, {
			"Country Code", "mpeg_descr.local_time_offset.country_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_region_id, {
			"Region ID", "mpeg_descr.local_time_offset.region_id",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOCAL_TIME_OFFSET_COUNTRY_REGION_ID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_reserved, {
			"Reserved", "mpeg_descr.local_time_offset.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOCAL_TIME_OFFSET_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_polarity, {
			"Time Offset Polarity", "mpeg_descr.local_time_offset.polarity",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_local_time_offset_polarity_vals),
			MPEG_DESCR_LOCAL_TIME_OFFSET_POLARITY, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_offset, {
			"Time Offset", "mpeg_descr.local_time_offset.offset",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_time_of_change, {
			"Time of Change", "mpeg_descr.local_time_offset.time_of_change",
			FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_local_time_offset_next_time_offset, {
			"Next Time Offset", "mpeg_descr.local_time_offset.next_time_offset",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_subtitling_lang_code, {
			"Language Code", "mpeg_descr.subtitling.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_subtitling_type, {
			"Subtitling Type", "mpeg_descr.subtitling.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_subtitling_type_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_subtitling_composition_page_id, {
			"Composition Page ID", "mpeg_descr.subtitling.composition_page_id",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_subtitling_ancillary_page_id, {
			"Ancillary Page ID", "mpeg_descr.subtitling.ancillary_page_id",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_terrestrial_delivery_centre_frequency, {
			"Centre Frequency", "mpeg_descr.terr_delivery.centre_freq",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_bandwidth, {
			"Bandwidth", "mpeg_descr.terr_delivery.bandwidth",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_bandwidth_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_BANDWIDTH_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_priority, {
			"Priority", "mpeg_descr.terr_delivery.priority",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_priority_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_PRIORITY_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_time_slicing_indicator, {
			"Time Slicing Indicator", "mpeg_descr.terr_delivery.time_slicing_ind",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_time_slicing_indicator_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_TIME_SLICING_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_mpe_fec_indicator, {
			"MPE-FEC Indicator", "mpeg_descr.terr_delivery.mpe_fec_ind",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_mpe_fec_indicator_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_MPE_FEC_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_reserved1, {
			"Reserved", "mpeg_descr.terr_delivery.reserved1",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_TERRESTRIAL_DELIVERY_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_constellation, {
			"Constellation", "mpeg_descr.terr_delivery.constellation",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_constellation_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_CONSTELLATION_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_hierarchy_information, {
			"Hierarchy Information", "mpeg_descr.terr_delivery.hierarchy_information",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_hierarchy_information_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_HIERARCHY_INFORMATION_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_code_rate_hp_stream, {
			"Code Rate High Priority Stream", "mpeg_descr.terr_delivery.code_rate_hp_stream",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_code_rate_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_CODE_RATE_HP_STREAM_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_code_rate_lp_stream, {
			"Code Rate Low Priority Stream", "mpeg_descr.terr_delivery.code_rate_lp_stream",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_code_rate_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_CODE_RATE_LP_STREAM_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_guard_interval, {
			"Guard Interval", "mpeg_descr.terr_delivery.guard_interval",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_guard_interval_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_GUARD_INTERVAL_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_transmission_mode, {
			"Transmission Mode", "mpeg_descr.terr_delivery.transmission_mode",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_transmission_mode_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_TRANSMISSION_MODE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_other_frequency_flag, {
			"Other Frequency Flag", "mpeg_descr.terr_delivery.other_freq_flag",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_terrestrial_delivery_other_frequency_flag_vals),
			MPEG_DESCR_TERRESTRIAL_DELIVERY_OTHER_FREQUENCY_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_terrestrial_delivery_reserved2, {
			"Reserved", "mpeg_descr.terr_delivery.reserved2",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },


		/
		{ &hf_mpeg_descr_private_data_specifier_id, {
			"Private Data Specifier", "mpeg_descr.private_data_specifier.id",
			FT_UINT32, BASE_HEX, VALS(mpeg_descr_data_specifier_id_vals), 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_data_bcast_bcast_id, {
			"Data Broadcast ID", "mpeg_descr.data_bcast.id",
			FT_UINT16, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_data_bcast_id_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_component_tag, {
			"Component Tag", "mpeg_descr.data_bcast.component_tag",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_selector_len, {
			"Selector Length", "mpeg_descr.data_bcast.selector_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_selector_bytes, {
			"Selector Bytes", "mpeg_descr.data_bcast.selector_bytes",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_lang_code, {
			"Language Code", "mpeg_descr.data_bcast.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_text_len, {
			"Text Length", "mpeg_descr.data_bcast.text_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_text, {
			"Text", "mpeg_descr.data_bcast.text",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_data_bcast_id_bcast_id, {
			"Data Broadcast ID", "mpeg_descr.data_bcast_id.id",
			FT_UINT16, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_data_bcast_id_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_data_bcast_id_id_selector_bytes, {
			"ID Selector Bytes", "mpeg_descr.data_bcast_id.id_selector_bytes",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_ac3_component_type_flag, {
			"Component Type Flag", "mpeg_descr.ac3.component_type_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_ac3_component_type_flag_vals),
			MPEG_DESCR_AC3_COMPONENT_TYPE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_bsid_flag, {
			"BSID Flag", "mpeg_descr.ac3.bsid_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_ac3_bsid_flag_vals),
			MPEG_DESCR_AC3_BSID_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_mainid_flag, {
			"Main ID Flag", "mpeg_descr.ac3_main_id_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_ac3_mainid_flag_vals),
			MPEG_DESCR_AC3_MAINID_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_asvc_flag, {
			"ASVC Flag", "mpeg_descr.ac3.asvc_flag",
			FT_UINT8, BASE_DEC, VALS(mpeg_descr_ac3_asvc_flag_vals),
			MPEG_DESCR_AC3_ASVC_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_reserved, {
			"Reserved", "mpeg_descr.ac3.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_AC3_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_component_type_reserved_flag, {
			"Type Reserved Flag", "mpeg_descr.ac3.component_type.reserved_flag",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_AC3_COMPONENT_TYPE_RESERVED_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_component_type_full_service_flag, {
			"Full Service Flag", "mpeg_descr.ac3.component_type.full_service_flag",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_ac3_component_type_full_service_flag_vals),
			MPEG_DESCR_AC3_COMPONENT_TYPE_FULL_SERVICE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_component_type_service_type_flags, {
			"Service Type Flags", "mpeg_descr.ac3.component_type.service_type_flags",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_ac3_component_type_service_type_flags_vals),
			MPEG_DESCR_AC3_COMPONENT_TYPE_SERVICE_TYPE_FLAGS_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_component_type_number_of_channels_flags, {
			"Number of Channels Flags", "mpeg_descr.ac3.component_type.number_chan_flags",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_ac3_component_type_number_of_channels_flags_vals),
			MPEG_DESCR_AC3_COMPONENT_TYPE_NUMBER_OF_CHANNELS_FLAGS, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_bsid, {
			"BSID", "mpeg_descr.ac3.bsid",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_mainid, {
			"Main ID", "mpeg_descr.ac3.mainid",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_asvc, {
			"ASVC", "mpeg_descr.ac3.asvc",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ac3_additional_info, {
			"Additional Info", "mpeg_descr.ac3.additional_info",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_app_sig_app_type, {
			"Application type", "mpeg_descr.app_sig.app_type",
			FT_UINT16, BASE_HEX, NULL, 0x7FFF, NULL, HFILL
		} },

		{ &hf_mpeg_descr_app_sig_ait_ver, {
			"AIT version", "mpeg_descr.app_sig.ait_ver",
			FT_UINT8, BASE_HEX, NULL, 0x3F, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_default_authority_name, {
			"Default Authority Name", "mpeg_descr.default_authority.name",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_content_identifier_crid_type, {
			"CRID Type", "mpeg_descr.content_identifier.crid_type",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_content_identifier_crid_type_vals),
			MPEG_DESCR_CONTENT_IDENTIFIER_CRID_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_identifier_crid_location, {
			"CRID Location", "mpeg_descr.content_identifier.crid_location",
			FT_UINT8, BASE_HEX, VALS(mpeg_descr_content_identifier_crid_location_vals),
			MPEG_DESCR_CONTENT_IDENTIFIER_CRID_LOCATION_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_identifier_crid_length, {
			"CRID Length", "mpeg_descr.content_identifier.crid_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_identifier_crid_bytes, {
			"CRID Bytes", "mpeg_descr.content_identifier.crid_bytes",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_content_identifier_cird_ref, {
			"CRID Reference", "mpeg_descr.content_identifier.crid_ref",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_extension_tag_extension, {
			"Descriptor Tag Extension", "mpeg_descr.ext.tag",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_extension_tag_extension_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extension_data, {
			"Descriptor Extension Data", "mpeg_descr.ext.data",
			FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_extension_supp_audio_mix_type, {
			"Mix type", "mpeg_descr.ext.supp_audio.mix_type",
			FT_UINT8, BASE_HEX, VALS(supp_audio_mix_type_vals), 0x80, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extension_supp_audio_ed_cla, {
			"Editorial classification", "mpeg_descr.ext.supp_audio.ed_cla",
			FT_UINT8, BASE_HEX, VALS(supp_audio_ed_cla), 0x7C, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extension_supp_audio_lang_code_present, {
			"Language code present", "mpeg_descr.ext.supp_audio.lang_code_present",
			FT_UINT8, BASE_HEX, NULL, 0x01, NULL, HFILL
		} },

		{ &hf_mpeg_descr_extension_supp_audio_lang_code, {
			"ISO 639 language code", "mpeg_descr.ext.supp_audio.lang_code",
			FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_logon_initialize_group_id, {
			"Group ID", "mpeg_descr.logon_init.group_id",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_logon_id, {
			"Logon ID", "mpeg_descr.logon_init.logon_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_continuous_carrier_reserved, {
			"Continuous Carrier Reserved", "mpeg_descr.logon_init.continuous_carrier_reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_CONTINUOUS_CARRIER_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_continuous_carrier, {
			"Continuous Carrier", "mpeg_descr.logon_init.continuous_carrier",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_CONTINUOUS_CARRIER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_security_handshake_required, {
			"Security Handshake Required", "mpeg_descr.logon_init.security_handshake_required",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_SECURITY_HANDSHAKE_REQUIRED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_prefix_flag, {
			"Prefix Flag", "mpeg_descr.logon_init.prefix_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_PREFIX_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_data_unit_labelling_flag, {
			"Unit Labelling Flag", "mpeg_descr.logon_init.data_unit_labelling_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_DATA_UNIT_LABELLING_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_mini_slot_flag, {
			"Mini Slot Flag", "mpeg_descr.logon_init.mini_slot_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_MINI_SLOT_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_contention_based_mini_slot_flag, {
			"Contention Based Mini Slot Flag", "mpeg_descr.logon_init.contention_based_mini_slot_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_CONTENTION_BASED_MINI_SLOT_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_capacity_type_flag_reserved, {
			"Capacity Type Flag Reserved", "mpeg_descr.logon_init.capactity_type_flag_reserved",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_CAPACITY_TYPE_FLAG_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_capacity_type_flag, {
			"Capacity Type Flag", "mpeg_descr.logon_init.capactity_type_flag",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_CAPACITY_TYPE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_traffic_burst_type, {
			"Traffic Burst Type", "mpeg_descr.logon_init.traffic_burst_type",
			FT_UINT8, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_TRAFFIC_BURST_TYPE_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_trf_pid, {
			"Return TRF PID", "mpeg_descr.logon_init.return_trf_pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_TRF_PID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_ctrl_mngm_pid_reserved, {
			"Return CTRL MNGM PID Reserved", "mpeg_descr.logon_init.return_mngm_pid_reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_CTRL_MNGM_PID_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_ctrl_mngm_pid, {
			"Return CTRL MNGM PID", "mpeg_descr.logon_init.return_mngm_pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_CTRL_MNGM_PID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_connectivity, {
			"Connectivity", "mpeg_descr.logon_init.connectivity",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_CONNECTIVITY_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_vpi_reserved, {
			"Return VPI Reserved", "mpeg_descr.logon_init.return_vpi_reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_VPI_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_vpi, {
			"Return VPI", "mpeg_descr.logon_init.return_vpi",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_VPI_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_vci, {
			"Return VCI", "mpeg_descr.logon_init.return_vci",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_signalling_vpi_reserved, {
			"Return Signalling VPI Reserved", "mpeg_descr.logon_init.return_signalling_vpi_reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_SIGNALLING_VPI_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_signalling_vpi, {
			"Return Signalling VPI", "mpeg_descr.logon_init.return_signalling_vpi",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_SIGNALLING_VPI_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_return_signalling_vci, {
			"Return Signalling VCI", "mpeg_descr.logon_init.return_signalling_vci",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_forward_signalling_vpi_reserved, {
			"Forward Signalling VPI Reserved", "mpeg_descr.logon_init.forward_signalling_vpi_reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_SIGNALLING_VPI_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_forward_signalling_vpi, {
			"Forward Signalling VPI", "mpeg_descr.logon_init.forward_signalling_vpi",
			FT_UINT8, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_RETURN_SIGNALLING_VPI_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_forward_signalling_vci, {
			"Forward Signalling VCI", "mpeg_descr.logon_init.forward_signalling_vci",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_cra_level, {
			"CRA Level", "mpeg_descr.logon_init.cra_level",
			FT_UINT24, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_vbdc_max_reserved, {
			"VDBC Max Reserved", "mpeg_descr.logon_init.vdbc_max_reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_DESCR_LOGON_INITIALIZE_VDBC_MAX_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_vbdc_max, {
			"VDBC Max", "mpeg_descr.logon_init.vdbc_max",
			FT_UINT16, BASE_DEC, NULL, MPEG_DESCR_LOGON_INITIALIZE_VDBC_MAX_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_rbdc_max, {
			"RDBC Max", "mpeg_descr.logon_init.rdbc_max",
			FT_UINT24, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_logon_initialize_rbdc_timeout, {
			"RDBC Timeout", "mpeg_descr.logon_init.rdbc_timeout",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_rcs_content_table_id, {
			"Table ID", "mpeg_descr.rcs_content.tid",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_ciplus_cl_cb_min, {
		   "Content byte minimum value", "mpeg_descr.ciplus_content_label.content_byte_min",
		   FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_cl_cb_max, {
		   "Content byte maximum value", "mpeg_descr.ciplus_content_label.content_byte_max",
		   FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_cl_lang, {
		 "ISO 639 language code", "mpeg_descr.ciplus_content_label.lang_code",
		 FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_cl_label, {
		  "Content label", "mpeg_descr.ciplus_content_label.label",
		  FT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		/
		{ &hf_mpeg_descr_ciplus_svc_id, {
			"Service ID", "mpeg_descr.ciplus_svc.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },
		
		{ &hf_mpeg_descr_ciplus_svc_type, {
			"Service type", "mpeg_descr.ciplus_svc.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_descr_service_type_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_svc_visible, {
			"Visible Service Flag", "mpeg_descr.ciplus_svc.visible",
			FT_UINT16, BASE_HEX, NULL, 0x8000, NULL, HFILL
		} },
			
		{ &hf_mpeg_descr_ciplus_svc_selectable, {
			"Selectable Service Flag", "mpeg_descr.ciplus_svc.selectable",
			FT_UINT16, BASE_HEX, NULL, 0x4000, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_svc_lcn, {
			"Logical Channel Number", "mpeg_descr.ciplus_svc.lcn",
			FT_UINT16, BASE_HEX, NULL, 0x3FFF, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_svc_prov_name, {
			"Service Provider Name", "mpeg_descr.ciplus_svc.provider_name",
			FT_UINT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_descr_ciplus_svc_name, {
			"Service Name", "mpeg_descr.ciplus_svc.name",
			FT_UINT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_mpeg_descriptor,
		&ett_mpeg_descriptor_extended_event_item,
		&ett_mpeg_descriptor_component_content_type,
		&ett_mpeg_descriptor_content_nibble,
		&ett_mpeg_descriptor_vbi_data_service,
		&ett_mpeg_descriptor_content_identifier_crid,
		&ett_mpeg_descriptor_service_list,
		&ett_mpeg_descriptor_ac3_component_type,
		&ett_mpeg_descriptor_linkage_population_id
	};

	proto_mpeg_descriptor = proto_register_protocol("MPEG2 Descriptors", "MPEG Descriptor", "mpeg_descr");
	proto_register_field_array(proto_mpeg_descriptor, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
