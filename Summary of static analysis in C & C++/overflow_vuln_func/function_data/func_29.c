void proto_reg_handoff_ua_msg(void)
{
	static gboolean prefs_initialized = FALSE;
	dissector_handle_t handle_ua_msg;

	if(!prefs_initialized)
	{
		/
		register_dissector_table("uaudp.opcode",
			"UAUDP opcode",
			FT_UINT8,
			BASE_DEC);

		handle_ua_msg = find_dissector("ua_sys_to_term");
		dissector_add_uint("uaudp.opcode", UAUDP_DATA, handle_ua_msg);

		handle_ua_msg = find_dissector("ua_term_to_sys");
		dissector_add_uint("uaudp.opcode", UAUDP_DATA, handle_ua_msg);
		prefs_initialized = TRUE;
	}
}
