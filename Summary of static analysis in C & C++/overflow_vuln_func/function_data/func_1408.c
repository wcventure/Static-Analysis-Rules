void
proto_register_dvb_sdt(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_sdt_transport_stream_id, {
			"Transport Stream ID", "dvb_sdt.tsid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_sdt_reserved1, {
			"Reserved", "dvb_sdt.reserved1",
			FT_UINT8, BASE_HEX, NULL, DVB_SDT_RESERVED1_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_version_number, {
			"Version Number", "dvb_sdt.version",
			FT_UINT8, BASE_HEX, NULL, DVB_SDT_VERSION_NUMBER_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_current_next_indicator, {
			"Current/Next Indicator", "dvb_sdt.cur_next_ind",
			FT_UINT8, BASE_DEC, VALS(dvb_sdt_cur_next_vals), DVB_SDT_CURRENT_NEXT_INDICATOR_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_section_number, {
			"Section Number", "dvb_sdt.sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_sdt_last_section_number, {
			"Last Section Number", "dvb_sdt.last_sect_num",
			FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_sdt_original_network_id, {
			"Original Network ID", "dvb_sdt.original_nid",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_sdt_reserved2, {
			"Reserved", "dvb_sdt.reserved2",
			FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL
		} },


		{ &hf_dvb_sdt_service_id, {
			"Service ID", "dvb_sdt.svc.id",
			FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_sdt_reserved3, {
			"Reserved", "dvb_sdt.svc.reserved",
			FT_UINT8, BASE_HEX, NULL, DVB_SDT_RESERVED3_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_eit_schedule_flag, {
			"EIT Schedule Flag", "dvb_sdt.svc.eit_schedule_flag",
			FT_UINT8, BASE_DEC, NULL, DVB_SDT_EIT_SCHEDULE_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_eit_present_following_flag, {
			"EIT Present Following Flag", "dvb_sdt.svc.eit_present_following_flag",
			FT_UINT8, BASE_DEC, NULL, DVB_SDT_EIT_PRESENT_FOLLOWING_FLAG_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_running_status, {
			"Running Status", "dvb_sdt.svc.running_status",
			FT_UINT16, BASE_HEX, VALS(dvb_sdt_running_status_vals), DVB_SDT_RUNNING_STATUS_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_free_ca_mode, {
			"Free CA Mode", "dvb_sdt.svc.free_ca_mode",
			FT_UINT16, BASE_HEX, VALS(dvb_sdt_free_ca_mode_vals), DVB_SDT_FREE_CA_MODE_MASK, NULL, HFILL
		} },

		{ &hf_dvb_sdt_descriptors_loop_length, {
			"Descriptors Loop Length", "dvb_sdt.svc.descr_loop_len",
			FT_UINT16, BASE_HEX, NULL, DVB_SDT_DESCRIPTORS_LOOP_LENGTH_MASK, NULL, HFILL
		} }

	};

	static gint *ett[] = {
		&ett_dvb_sdt,
		&ett_dvb_sdt_service
	};

	proto_dvb_sdt = proto_register_protocol("DVB Service Description Table", "DVB SDT", "dvb_sdt");

	proto_register_field_array(proto_dvb_sdt, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
