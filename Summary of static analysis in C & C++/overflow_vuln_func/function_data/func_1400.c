void
proto_reg_handoff_dvb_data_mpe(void)
{

	dissector_handle_t dvb_data_mpe_handle;

	dvb_data_mpe_handle = create_dissector_handle(dissect_dvb_data_mpe, proto_dvb_data_mpe);
	dissector_add_uint("mpeg_sect.tid", DVB_DATA_MPE_TID, dvb_data_mpe_handle);

	ip_handle  = find_dissector("ip");
	llc_handle = find_dissector("llc");

}
