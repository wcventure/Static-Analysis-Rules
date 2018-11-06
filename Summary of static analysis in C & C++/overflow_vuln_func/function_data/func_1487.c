void
proto_register_mpeg_sect(void)
{
	static hf_register_info hf[] = {
		{ &hf_mpeg_sect_table_id, {
			"Table ID", "mpeg_sect.tid",
			FT_UINT8, BASE_HEX, VALS(mpeg_sect_table_id_vals), 0, NULL, HFILL
		} },

		{ &hf_mpeg_sect_syntax_indicator, {
			"Syntax indicator", "mpeg_sect.syntax_indicator",
			FT_UINT16, BASE_DEC, NULL, MPEG_SECT_SYNTAX_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_sect_reserved, {
			"Reserved", "mpeg_sect.reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_SECT_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_sect_length, {
			"Length", "mpeg_sect.len",
			FT_UINT16, BASE_DEC, NULL, MPEG_SECT_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_sect_crc, {
			"CRC 32", "mpeg_sect.crc",
			FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_mpeg_sect
	};
	module_t *mpeg_sect_module;

	proto_mpeg_sect = proto_register_protocol("MPEG2 Section", "MPEG SECT", "mpeg_sect");
	register_dissector("mpeg_sect", dissect_mpeg_sect, proto_mpeg_sect);

	proto_register_field_array(proto_mpeg_sect, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	mpeg_sect_module = prefs_register_protocol(proto_mpeg_sect, NULL);

	prefs_register_bool_preference(mpeg_sect_module,
		"verify_crc",
		"Verify the section CRC",
		"Whether the section dissector should verify the CRC",
		&mpeg_sect_check_crc);

	mpeg_sect_tid_dissector_table = register_dissector_table("mpeg_sect.tid",
								 "MPEG SECT Table ID",
								 FT_UINT8, BASE_HEX);

}
