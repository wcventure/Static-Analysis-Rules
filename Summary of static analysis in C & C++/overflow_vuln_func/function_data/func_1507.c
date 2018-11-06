void proto_reg_handoff_jxta(void)
{
    static gboolean init_done = FALSE;

    if (!init_done) {
        new_register_dissector("jxta.udp", dissect_jxta_udp, proto_jxta);
        heur_dissector_add("udp", dissect_jxta_UDP_heur, proto_jxta);

        new_register_dissector("jxta.tcp", dissect_jxta_tcp, proto_jxta);
        tcp_jxta_handle = find_dissector("jxta.tcp");
        heur_dissector_add("tcp", dissect_jxta_TCP_heur, proto_jxta);

        init_done = TRUE;
    }
}
