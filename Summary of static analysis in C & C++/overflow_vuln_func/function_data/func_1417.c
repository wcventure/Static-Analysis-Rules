void
proto_register_mpeg_ca(void)
{

	static hf_register_info hf[] = {

		{ &hf_mpeg_ca_reserved, {
			"Reserved", "mpeg_ca.reserved",
			FT_UINT24, BASE_HEX, NULL, MPEG_CA_RESERVED_MASK,
                        NULL, HFILL
		} },

		{ &hf_mpeg_ca_version_number, {
			"Version Number", "mpeg_ca.version",
			FT_UINT24, BASE_HEX, NULL, MPEG_CA_VERSION_NUMBER_MASK,
                        NULL, HFILL
		} },

		{ &hf_mpeg_ca_current_next_indicator, {
			"Current/Next Indicator", "mpeg_ca.cur_next_ind",
			FT_UINT24, BASE_HEX, VALS(mpeg_ca_cur_next_vals), MPEG_CA_CURRENT_NEXT_INDICATOR_MASK,
                        NULL, HFILL
		} },

		{ &hf_mpeg_ca_section_number, {
			"Section Number", "mpeg_ca.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0,
                        NULL, HFILL
		} },

		{ &hf_mpeg_ca_last_section_number, {
			"Last Section Number", "mpeg_ca.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0,
                        NULL, HFILL
		} },

	};

	static gint *ett[] = {
		&ett_mpeg_ca,
	};

	proto_mpeg_ca = proto_register_protocol("MPEG2 Conditional Access Table", "MPEG CA", "mpeg_ca");

	proto_register_field_array(proto_mpeg_ca, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
