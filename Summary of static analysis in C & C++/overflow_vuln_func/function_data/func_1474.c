void
proto_register_dsmcc(void)
{
	/
	static hf_register_info hf[] = {
		/
		{ &hf_dsmcc_protocol_discriminator, {
			"Protocol Discriminator", "mpeg_dsmcc.protocol",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_type, {
			"Type", "mpeg_dsmcc.type",
			FT_UINT8, BASE_HEX|BASE_RANGE_STRING, RVALS(dsmcc_header_type_vals), 0, NULL, HFILL
		} },

		{ &hf_dsmcc_message_id, {
			"Message ID", "mpeg_dsmcc.message_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_transaction_id, {
			"Transaction ID", "mpeg_dsmcc.transaction_id",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_header_reserved, {
			"Reserved", "mpeg_dsmcc.header_reserved",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_adaptation_length, {
			"Adaptation Length", "mpeg_dsmcc.adaptation_length",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_message_length, {
			"Message Length", "mpeg_dsmcc.message_length",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_adaptation_type, {
			"Adaptation Type", "mpeg_dsmcc.adaptation_header.type",
			FT_UINT8, BASE_HEX|BASE_RANGE_STRING, RVALS(dsmcc_adaptation_header_vals), 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_adaptation_ca_reserved, {
			"Reserved", "mpeg_dsmcc.adaptation_header.ca.reserved",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_adaptation_ca_system_id, {
			"System ID", "mpeg_dsmcc.adaptation_header.ca.system_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_adaptation_ca_length, {
			"System ID", "mpeg_dsmcc.adaptation_header.ca.length",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_adaptation_user_id_reserved, {
			"Reserved", "mpeg_dsmcc.adaptation_header.uid.reserved",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_compat_desc_length, {
			"Compatibility Descriptor Length", "mpeg_dsmcc.dii.compat_desc_len",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_compat_desc_count, {
			"Descriptor Length", "mpeg_dsmcc.dii.compat_desc_count",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_type, {
			"Descriptor Type", "mpeg_dsmcc.dii.compat.type",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_length, {
			"Descriptor Length", "mpeg_dsmcc.dii.compat.length",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_spec_type, {
			"Specifier Type", "mpeg_dsmcc.dii.compat.spec_type",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_spec_data, {
			"Specifier Data", "mpeg_dsmcc.dii.compat.spec_data",
			FT_UINT24, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_model, {
			"Model", "mpeg_dsmcc.dii.compat.model",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_version, {
			"Version", "mpeg_dsmcc.dii.compat.version",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_sub_desc_count, {
			"Version", "mpeg_dsmcc.dii.compat.sub_count",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_sub_desc_type, {
			"Type", "mpeg_dsmcc.dii.compat.sub_type",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_desc_sub_desc_len, {
			"Length", "mpeg_dsmcc.dii.compat.sub_len",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_dd_download_id, {
			"Download ID", "mpeg_dsmcc.download_id",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dd_message_id, {
			"Message ID", "mpeg_dsmcc.message_id",
			FT_UINT16, BASE_HEX, VALS(dsmcc_dd_message_id_vals), 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_dii_download_id, {
			"Download ID", "mpeg_dsmcc.dii.download_id",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_block_size, {
			"Block Size", "mpeg_dsmcc.dii.block_size",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_window_size, {
			"Window Size", "mpeg_dsmcc.dii.window_size",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_ack_period, {
			"ACK Period", "mpeg_dsmcc.dii.ack_period",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_t_c_download_window, {
			"Carousel Download Window", "mpeg_dsmcc.dii.carousel_download_window",
			FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_t_c_download_scenario, {
			"Carousel Download Scenario", "mpeg_dsmcc.dii.carousel_download_scenario",
			FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_number_of_modules, {
			"Number of Modules", "mpeg_dsmcc.dii.module_count",
			FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_module_id, {
			"Module ID", "mpeg_dsmcc.dii.module_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_module_size, {
			"Module Size", "mpeg_dsmcc.dii.module_size",
			FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_module_version, {
			"Module Version", "mpeg_dsmcc.dii.module_version",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_module_info_length, {
			"Module Info Length", "mpeg_dsmcc.dii.module_info_length",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_dii_private_data_length, {
			"Private Data Length", "mpeg_dsmcc.dii.private_data_length",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_ddb_module_id, {
			"Module ID", "mpeg_dsmcc.ddb.module_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_ddb_version, {
			"Version", "mpeg_dsmcc.ddb.version",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_ddb_reserved, {
			"Reserved", "mpeg_dsmcc.ddb.reserved",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_ddb_block_number, {
			"Block Number", "mpeg_dsmcc.ddb.block_num",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },
		/


		/
		{ &hf_dsmcc_table_id, {
			"Table ID", "mpeg_sect.table_id",
			FT_UINT8, BASE_HEX, VALS(dsmcc_payload_name_vals), 0, NULL, HFILL
		} },

		{ &hf_dsmcc_section_syntax_indicator, {
			"Session Syntax Indicator", "mpeg_sect.ssi",
			FT_UINT16, BASE_DEC, NULL, DSMCC_SSI_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_private_indicator, {
			"Private Indicator", "mpeg_dsmcc.private_indicator",
			FT_UINT16, BASE_DEC, NULL, DSMCC_PRIVATE_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_reserved, {
			"Reserved", "mpeg_sect.reserved",
			FT_UINT16, BASE_HEX, NULL, DSMCC_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_section_length, {
			"Length", "mpeg_sect.section_length",
			FT_UINT16, BASE_DEC, NULL, DSMCC_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_table_id_extension, {
			"Table ID Extension", "mpeg_dsmcc.table_id_extension",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_reserved2, {
			"Reserved", "mpeg_dsmcc.reserved2",
			FT_UINT8, BASE_HEX, NULL, DSMCC_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_version_number, {
			"Version Number", "mpeg_dsmcc.version_number",
			FT_UINT8, BASE_DEC, NULL, DSMCC_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_current_next_indicator, {
			"Current Next Indicator", "mpeg_dsmcc.current_next_indicator",
			FT_UINT8, BASE_DEC, NULL, DSMCC_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_dsmcc_section_number, {
			"Section Number", "mpeg_dsmcc.section_number",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_last_section_number, {
			"Last Section Number", "mpeg_dsmcc.last_section_number",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_crc, {
			"CRC 32", "mpeg_sect.crc",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dsmcc_checksum, {
			"Checksum", "mpeg_dsmcc.checksum",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} },
		/


		{ &hf_etv_module_abs_path, {
			"Module Absolute Path", "etv.dsmcc.dii.module_abs_path",
			FT_UINT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} },

		{ &hf_etv_dii_authority, {
			"Authority", "etv.dsmcc.dii.authority",
			FT_UINT_STRING, BASE_NONE, NULL, 0, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_dsmcc,
		&ett_dsmcc_payload,
		&ett_dsmcc_adaptation_header,
		&ett_dsmcc_header,
		&ett_dsmcc_compat,
		&ett_dsmcc_compat_sub_desc,
		&ett_dsmcc_dii_module
	};
	module_t *dsmcc_module;

	proto_dsmcc = proto_register_protocol("MPEG DSM-CC", "MPEG DSM-CC", "mpeg_dsmcc");

	proto_register_field_array(proto_dsmcc, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
	new_register_dissector("mp2t-dsmcc", dissect_dsmcc_ts, proto_dsmcc);

	dsmcc_module = prefs_register_protocol(proto_dsmcc, NULL);

	prefs_register_bool_preference(dsmcc_module, "verify_crc",
		"Verify the section CRC or checksum",
		"Whether the section dissector should verify the CRC or checksum",
		&dsmcc_sect_check_crc);
}
