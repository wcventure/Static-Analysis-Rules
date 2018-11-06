void
proto_register_dvb_eit(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_eit_service_id, {
			"Service ID", "dvb_eit.sid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_reserved, {
			"Reserved", "dvb_eit.reserved",
			FT_UINT8, BASE_HEX, NULL, DVB_EIT_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_dvb_eit_version_number, {
			"Version Number", "dvb_eit.version",
			FT_UINT8, BASE_HEX, NULL, DVB_EIT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_dvb_eit_current_next_indicator, {
			"Current/Next Indicator", "dvb_eit.cur_next_ind",
			FT_UINT8, BASE_DEC, VALS(dvb_eit_cur_next_vals), DVB_EIT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_dvb_eit_section_number, {
			"Section Number", "dvb_eit.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_last_section_number, {
			"Last Section Number", "dvb_eit.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_transport_stream_id, {
			"Transport Stream ID", "dvb_eit.tsid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_original_network_id, {
			"Original Network ID", "dvb_eit.original_nid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_segment_last_section_number, {
			"Segment Last Section Number", "dvb_eit.segment_last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_last_table_id, {
			"Last Table ID", "dvb_eit.last_tid",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_event_id, {
			"Event ID", "dvb_eit.evt.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_start_time, {
			"UTC Start Time", "dvb_eit.evt.start_time",
			FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_duration, {
			"Duration", "dvb_eit.evt.duration",
			FT_UINT24, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_eit_running_status, {
			"Running Status", "dvb_eit.evt.running_status",
			FT_UINT16, BASE_HEX, VALS(dvb_eit_running_status_vals), DVB_EIT_RUNNING_STATUS_MASK, NULL, HFILL
		} },

		{ &hf_dvb_eit_free_ca_mode, {
			"Free CA Mode", "dvb_eit.evt.free_ca_mode",
			FT_UINT16, BASE_HEX, VALS(dvb_eit_free_ca_mode_vals), DVB_EIT_FREE_CA_MODE_MASK, NULL, HFILL
		} },

		{ &hf_dvb_eit_descriptors_loop_length, {
			"Descriptors Loop Length", "dvb_eit.evt.descr_loop_len",
			FT_UINT16, BASE_HEX, NULL, DVB_EIT_DESCRIPTORS_LOOP_LENGTH_MASK, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_dvb_eit,
		&ett_dvb_eit_event
	};

	proto_dvb_eit = proto_register_protocol("DVB Event Information Table", "DVB EIT", "dvb_eit");

	proto_register_field_array(proto_dvb_eit, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
