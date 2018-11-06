void
proto_register_dvb_tot(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_tot_utc_time, {
			"UTC Time", "dvb_tot.utc_time",
			FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0, NULL, HFILL
		} },

		{ &hf_dvb_tot_reserved, {
			"Reserved", "dvb_tot.reserved",
			FT_UINT16, BASE_HEX, NULL, DVB_TOT_RESERVED_MASK, NULL, HFILL
		} },

		{ &hf_dvb_tot_descriptors_loop_length, {
			 "Descriptors Loop Length", "dvb_tot.descr_loop_len",
			 FT_UINT16, BASE_DEC, NULL, DVB_TOT_DESCRIPTORS_LOOP_LENGTH_MASK, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_dvb_tot
	};

	proto_dvb_tot = proto_register_protocol("DVB Time Offset Table", "DVB TOT", "dvb_tot");

	proto_register_field_array(proto_dvb_tot, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
