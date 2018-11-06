void proto_reg_handoff_dvb_eit(void)
{
	int tid;
	dissector_handle_t dvb_eit_handle;

	dvb_eit_handle = create_dissector_handle(dissect_dvb_eit, proto_dvb_eit);

	for (tid = DVB_EIT_TID_MIN; tid <= DVB_EIT_TID_MAX; tid++)
		dissector_add_uint("mpeg_sect.tid", tid, dvb_eit_handle);
}
