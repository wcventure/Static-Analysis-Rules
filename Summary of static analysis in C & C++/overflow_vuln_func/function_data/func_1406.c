void proto_reg_handoff_dvb_nit(void)
{
	dissector_handle_t dvb_nit_handle;

	dvb_nit_handle = new_create_dissector_handle(dissect_dvb_nit, proto_dvb_nit);

	dissector_add_uint("mpeg_sect.tid", DVB_NIT_TID, dvb_nit_handle);
	dissector_add_uint("mpeg_sect.tid", DVB_NIT_TID_OTHER, dvb_nit_handle);
}
