void
proto_register_dvb_nit(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_nit_network_id, {
			"Network ID", "dvb_nit.sid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_nit_reserved1, {
			"Reserved", "dvb_nit.reserved1",
			FT_UINT8, BASE_HEX, NULL, DVB_NIT_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_version_number, {
			"Version Number", "dvb_nit.version",
			FT_UINT8, BASE_HEX, NULL, DVB_NIT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_current_next_indicator, {
			"Current/Next Indicator", "dvb_nit.cur_next_ind",
			FT_UINT8, BASE_DEC, VALS(dvb_nit_cur_next_vals), DVB_NIT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_section_number, {
			"Section Number", "dvb_nit.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_nit_last_section_number, {
			"Last Section Number", "dvb_nit.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_nit_reserved2, {
			"Reserved", "dvb_nit.reserved2",
			FT_UINT16, BASE_HEX, NULL, DVB_NIT_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_network_descriptors_length, {
			"Network Descriptors Length", "dvb_nit.network_desc_len",
			FT_UINT16, BASE_DEC, NULL, DVB_NIT_NETWORK_DESCRIPTORS_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_reserved3, {
			"Reserved", "dvb_nit.reserved3",
			FT_UINT16, BASE_HEX, NULL, DVB_NIT_RESERVED3_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_transport_stream_loop_length, {
			"Transport Stream Loop Length", "dvb_nit.ts_loop_len",
			FT_UINT16, BASE_DEC, NULL, DVB_NIT_TRANSPORT_STREAM_LOOP_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_transport_stream_id, {
			"Transport Stream ID", "dvb_nit.ts.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_nit_original_network_id, {
			"Original Network ID", "dvb_nit.ts.original_network_id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_nit_reserved4, {
			"Reserved", "dvb_nit.ts.reserved",
			FT_UINT16, BASE_HEX, NULL, DVB_NIT_RESERVED4_MASK, NULL, HFILL
		} },

		{ &hf_dvb_nit_transport_descriptors_length, {
			"Transport Descriptors Length", "dvb_nit.ts.desc_len",
			FT_UINT16, BASE_DEC, NULL, DVB_NIT_TRANSPORT_DESCRIPTORS_LENGTH_MASK, NULL, HFILL
		} },

	};

	static gint *ett[] = {
		&ett_dvb_nit,
		&ett_dvb_nit_ts
	};

	proto_dvb_nit = proto_register_protocol("DVB Network Information Table", "DVB NIT", "dvb_nit");

	proto_register_field_array(proto_dvb_nit, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	new_register_dissector("dvb_nit", dissect_dvb_nit, proto_dvb_nit);
}
