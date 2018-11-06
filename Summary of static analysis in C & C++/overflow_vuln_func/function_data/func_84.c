void proto_reg_handoff_uasip(void)
{
	static gboolean prefs_initialized = FALSE;
	static dissector_handle_t uasip_handle;

	if(!prefs_initialized)
	{
		uasip_handle = create_dissector_handle(dissect_uasip, proto_uasip);
		ua_sys_to_term_handle = find_dissector("ua_sys_to_term");
		ua_term_to_sys_handle = find_dissector("ua_term_to_sys");
		prefs_initialized = TRUE;
	}
	else 
	{
		if (str_to_ipaddr(pref_proxy_ipaddr_s, proxy_ipaddr))
		{
			use_proxy_ipaddr = TRUE;
		}
		else
		{
			use_proxy_ipaddr = FALSE;
			pref_proxy_ipaddr_s = g_strdup("");
		}
	}
	dissector_add_string("media_type", "application/octet-stream", uasip_handle);
}
