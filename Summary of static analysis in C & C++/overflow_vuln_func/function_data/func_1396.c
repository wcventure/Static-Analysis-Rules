void
proto_register_dvb_bat(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_bat_bouquet_id, {
			"Service ID", "dvb_bat.sid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_bat_reserved1, {
			"Reserved", "dvb_bat.reserved1",
			FT_UINT8, BASE_HEX, NULL, DVB_BAT_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_version_number, {
			"Version Number", "dvb_bat.version",
			FT_UINT8, BASE_HEX, NULL, DVB_BAT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_current_next_indicator, {
			"Current/Next Indicator", "dvb_bat.cur_next_ind",
			FT_UINT8, BASE_DEC, VALS(dvb_bat_cur_next_vals), DVB_BAT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_section_number, {
			"Section Number", "dvb_bat.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_bat_last_section_number, {
			"Last Section Number", "dvb_bat.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_bat_reserved2, {
			"Reserved", "dvb_bat.reserved2",
			FT_UINT16, BASE_HEX, NULL, DVB_BAT_RESERVED2_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_bouquet_descriptors_length, {
			"Bouquet Descriptors Length", "dvb_bat.bouquet_desc_len",
			FT_UINT16, BASE_DEC, NULL, DVB_BAT_BOUQUET_DESCRIPTORS_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_reserved3, {
			"Reserved", "dvb_bat.reserved3",
			FT_UINT16, BASE_HEX, NULL, DVB_BAT_RESERVED3_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_transport_stream_loop_length, {
			"Transport Stream Loop Length", "dvb_bat.ts_loop_len",
			FT_UINT16, BASE_DEC, NULL, DVB_BAT_TRANSPORT_STREAM_LOOP_LENGTH_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_transport_stream_id, {
			"Transport Stream ID", "dvb_bat.ts.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_bat_original_network_id, {
			"Original Network ID", "dvb_bat.ts.original_nid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_bat_reserved4, {
			"Reserved", "dvb_bat.ts.reserved",
			FT_UINT16, BASE_HEX, NULL, DVB_BAT_RESERVED4_MASK, NULL, HFILL
		} },

		{ &hf_dvb_bat_transport_descriptors_length, {
			"Bouquet Descriptors Length", "dvb_bat.ts.desc_len",
			FT_UINT16, BASE_DEC, NULL, DVB_BAT_BOUQUET_DESCRIPTORS_LENGTH_MASK, NULL, HFILL
		} },

	};

	static gint *ett[] = {
		&ett_dvb_bat,
		&ett_dvb_bat_transport_stream
	};

	proto_dvb_bat = proto_register_protocol("DVB Bouquet Association Table", "DVB BAT", "dvb_bat");

	proto_register_field_array(proto_dvb_bat, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
