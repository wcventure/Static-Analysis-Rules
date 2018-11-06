void
proto_register_mpeg_pat(void)
{

	static hf_register_info hf[] = {

		{ &hf_mpeg_pat_transport_stream_id, {
			"Transport Stream ID", "mpeg_pat.tsid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pat_reserved, {
			"Reserved", "mpeg_pat.reserved",
			FT_UINT8, BASE_HEX, NULL, MPEG_PAT_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pat_version_number, {
			"Version Number", "mpeg_pat.version",
			FT_UINT8, BASE_HEX, NULL, MPEG_PAT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pat_current_next_indicator, {
			"Current/Next Indicator", "mpeg_pat.cur_next_ind",
			FT_BOOLEAN, 8, TFS(&mpeg_pat_cur_next_vals), MPEG_PAT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pat_section_number, {
			"Section Number", "mpeg_pat.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pat_last_section_number, {
			"Last Section Number", "mpeg_pat.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pat_program_number, {
			"Program Number", "mpeg_pat.prog_num",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pat_program_reserved, {
			"Reserved", "mpeg_pat.prog_reserved",
			FT_UINT16, BASE_HEX, NULL, MPEG_PAT_PROGRAM_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pat_program_map_pid, {
			"Program Map PID", "mpeg_pat.prog_map_pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_PAT_PROGRAM_MAP_PID_MASK, NULL, HFILL
		} },

	};

	static gint *ett[] = {
		&ett_mpeg_pat,
		&ett_mpeg_pat_prog
	};

	proto_mpeg_pat = proto_register_protocol("MPEG2 Program Association Table", "MPEG PAT", "mpeg_pat");

	proto_register_field_array(proto_mpeg_pat, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
