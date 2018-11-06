void
proto_register_mpeg_pmt(void)
{

	static hf_register_info hf[] = {

		{ &hf_mpeg_pmt_program_number, {
			"Program Number", "mpeg_pmt.pg_num",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_reserved1, {
			"Reserved", "mpeg_pmt.reserved1",
			FT_UINT8, BASE_HEX, NULL, MPEG_PMT_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_version_number, {
			"Version Number", "mpeg_pmt.version",
			FT_UINT8, BASE_HEX, NULL, MPEG_PMT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_current_next_indicator, {
			"Current/Next Indicator", "mpeg_pmt.cur_next_ind",
			FT_UINT8, BASE_HEX, VALS(mpeg_pmt_cur_next_vals), MPEG_PMT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_section_number, {
			"Section Number", "mpeg_pmt.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_last_section_number, {
			"Last Section Number", "mpeg_pmt.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_reserved2, {
			"Reserved", "mpeg_pmt.reserved2",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_pcr_pid, {
			"PCR PID", "mpeg_pmt.pcr_pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_PCR_PID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_reserved3, {
			"Reserved", "mpeg_pmt.reserved3",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_RESERVED3_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_program_info_length, {
			"Program Info Length", "mpeg_pmt.prog_info_len",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_PROGRAM_INFO_LENGTH_MASK, NULL, HFILL
		} },


		{ &hf_mpeg_pmt_stream_type, {
			"Stream type", "mpeg_pmt.stream.type",
			FT_UINT8, BASE_HEX | BASE_EXT_STRING, &mpeg_pmt_stream_type_vals_ext, 0, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_stream_reserved1, {
			"Reserved", "mpeg_pmt.stream.reserved1",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_STREAM_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_stream_elementary_pid, {
			"Elementary PID", "mpeg_pmt.stream.elementary_pid",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_STREAM_ELEMENTARY_PID_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_stream_reserved2, {
			"Reserved", "mpeg_pmt.stream.reserved2",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_STREAM_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_mpeg_pmt_stream_es_info_length, {
			"ES Info Length", "mpeg_pmt.stream.es_info_len",
			FT_UINT16, BASE_HEX, NULL, MPEG_PMT_STREAM_ES_INFO_LENGTH_MASK, NULL, HFILL
		} },

	};

	static gint *ett[] = {
		&ett_mpeg_pmt,
		&ett_mpeg_pmt_stream,
	};

	proto_mpeg_pmt = proto_register_protocol("MPEG2 Program Map Table", "MPEG PMT", "mpeg_pmt");

	proto_register_field_array(proto_mpeg_pmt, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	new_register_dissector("mpeg_pmt", dissect_mpeg_pmt, proto_mpeg_pmt);
}
