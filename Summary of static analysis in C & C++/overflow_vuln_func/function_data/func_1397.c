void proto_reg_handoff_dvb_bat(void)
{
	dissector_handle_t dvb_bat_handle;

	dvb_bat_handle = create_dissector_handle(dissect_dvb_bat, proto_dvb_bat);

	dissector_add_uint("mpeg_sect.tid", DVB_BAT_TID, dvb_bat_handle);
}
