void proto_reg_handoff_mpeg_ca(void)
{
	dissector_handle_t mpeg_ca_handle;

	mpeg_ca_handle = create_dissector_handle(dissect_mpeg_ca, proto_mpeg_ca);
	dissector_add_uint("mpeg_sect.tid", MPEG_CA_TID, mpeg_ca_handle);
}
