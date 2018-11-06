void proto_reg_handoff_uaudp(void)
{
    static gboolean           prefs_initialized = FALSE;
    int i;

    if (!prefs_initialized)
    {
        ua_sys_to_term_handle = find_dissector("ua_sys_to_term");
        ua_term_to_sys_handle = find_dissector("ua_term_to_sys");
#if 0
        uaudp_opcode_dissector_table =
            register_dissector_table("uaudp.opcode",
                                     "UAUDP opcode",
                                     FT_UINT8,
                                     BASE_DEC, DISSECTOR_TABLE_NOT_ALLOW_DUPLICATE);
#endif
        prefs_initialized     = TRUE;
    }
    else
    {
        for (i=0; i<MAX_TERMINAL_PORTS; i++)
        {
            if (ports[i].last_port)
                dissector_delete_uint("udp.port", ports[i].last_port, uaudp_handle);
        }
        if (str_to_addr_ip(pref_sys_ip_s, sys_ip))
        {
            use_sys_ip = TRUE;
        }
        else
        {
            use_sys_ip = FALSE;
            pref_sys_ip_s = "";
        }
    }

    if (decode_ua)
    {
        int no_ports_registered = TRUE;

        for (i=0; i < MAX_TERMINAL_PORTS; i++)
        {
            if (ports[i].port)
            {
                dissector_add_uint("udp.port", ports[i].port, uaudp_handle);
                no_ports_registered = FALSE;
            }
            ports[i].last_port = ports[i].port;
        }

        if (no_ports_registered)
        {
            /
            dissector_add_for_decode_as("udp.port", uaudp_handle);
        }
    }
}
