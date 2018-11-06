void
proto_reg_handoff_mpeg_pmt(void)
{
	dissector_handle_t mpeg_pmt_handle;

	mpeg_pmt_handle = new_create_dissector_handle(dissect_mpeg_pmt, proto_mpeg_pmt);
	dissector_add_uint("mpeg_sect.tid", MPEG_PMT_TID, mpeg_pmt_handle);
}
