void proto_reg_handoff_dvb_sdt(void)
{
	dissector_handle_t dvb_sdt_handle;

	dvb_sdt_handle = create_dissector_handle(dissect_dvb_sdt, proto_dvb_sdt);
	dissector_add_uint("mpeg_sect.tid", DVB_SDT_TID_ACTUAL, dvb_sdt_handle);
	dissector_add_uint("mpeg_sect.tid", DVB_SDT_TID_OTHER, dvb_sdt_handle);
}
