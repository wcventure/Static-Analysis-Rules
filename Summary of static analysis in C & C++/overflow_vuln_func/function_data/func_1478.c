void proto_reg_handoff_mpeg_pat(void)
{
	dissector_handle_t mpeg_pat_handle;

	mpeg_pat_handle = create_dissector_handle(dissect_mpeg_pat, proto_mpeg_pat);
	dissector_add_uint("mpeg_sect.tid", MPEG_PAT_TID, mpeg_pat_handle);
}
