void
proto_register_dvb_tdt(void)
{

	static hf_register_info hf[] = {

		{ &hf_dvb_tdt_utc_time, {
			"UTC Time", "dvb_tdt.utc_time",
			FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0, NULL, HFILL
		} }
	};

	static gint *ett[] = {
		&ett_dvb_tdt
	};

	proto_dvb_tdt = proto_register_protocol("DVB Time and Date Table", "DVB TDT", "dvb_tdt");

	proto_register_field_array(proto_dvb_tdt, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}
