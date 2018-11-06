void
proto_reg_handoff_uaudp(void)
{
	static gboolean prefs_initialized = FALSE;
	static dissector_handle_t uaudp_handle;
	int i;

	if(!prefs_initialized)
	{
		uaudp_handle = create_dissector_handle(dissect_uaudp, proto_uaudp);
		ua_sys_to_term_handle = find_dissector("ua_sys_to_term");
		ua_term_to_sys_handle = find_dissector("ua_term_to_sys");
		prefs_initialized = TRUE;
	}
	else
	{
		for(i=0; i<MAX_TERMINAL_PORTS; i++)
		{
			dissector_delete_uint("udp.port", ports[i].last_port, uaudp_handle);
		}
		if(str_to_addr_ip(pref_sys_ip_s, sys_ip))
		{
			use_sys_ip = TRUE;
		}
		else 
		{
			use_sys_ip = FALSE;
			pref_sys_ip_s = g_strdup("");
		}
	}

	if(decode_ua)
	{
		for(i=0; i < MAX_TERMINAL_PORTS; i++)
		{
			dissector_add_uint("udp.port", ports[i].port, uaudp_handle);
			ports[i].last_port = ports[i].port;
		}
	}
}
