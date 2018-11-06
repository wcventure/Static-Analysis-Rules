void proto_reg_handoff_dvb_tdt(void)
{
	dissector_handle_t dvb_tdt_handle;

	dvb_tdt_handle = create_dissector_handle(dissect_dvb_tdt, proto_dvb_tdt);

	dissector_add_uint("mpeg_sect.tid", DVB_TDT_TID, dvb_tdt_handle);
}
