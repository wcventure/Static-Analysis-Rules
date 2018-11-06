void proto_reg_handoff_dvb_tot(void)
{
	dissector_handle_t dvb_tot_handle;

	dvb_tot_handle = create_dissector_handle(dissect_dvb_tot, proto_dvb_tot);

	dissector_add_uint("mpeg_sect.tid", DVB_TOT_TID, dvb_tot_handle);
}
