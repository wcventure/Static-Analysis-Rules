void
proto_reg_handoff_dsmcc(void)
{
	dissector_handle_t dsmcc_handle;

	dsmcc_handle = create_dissector_handle(dissect_dsmcc, proto_dsmcc);
	dissector_add_uint("mpeg_sect.tid", DSMCC_TID_LLCSNAP, dsmcc_handle);
	dissector_add_uint("mpeg_sect.tid", DSMCC_TID_UN_MSG, dsmcc_handle);
	dissector_add_uint("mpeg_sect.tid", DSMCC_TID_DD_MSG, dsmcc_handle);
	dissector_add_uint("mpeg_sect.tid", DSMCC_TID_DESC_LIST, dsmcc_handle);
	dissector_add_uint("mpeg_sect.tid", DSMCC_TID_PRIVATE, dsmcc_handle);
	data_handle = find_dissector("data");
}
