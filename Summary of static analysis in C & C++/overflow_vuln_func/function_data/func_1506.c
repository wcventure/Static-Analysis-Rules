void proto_register_jxta(void)
{
    module_t *jxta_module;

    /

    register_init_routine( &jxta_dissector_init);

    proto_jxta = proto_register_protocol("JXTA P2P", "JXTA", "jxta");

    jxta_tap = register_tap("jxta");

    proto_message_jxta = proto_register_protocol("Media Type: application/x-jxta-msg",
                                                 "application/x-jxta-msg", "application-x-jxta-msg");

    /
    proto_register_field_array(proto_jxta, hf, array_length(hf));

    /
    proto_register_subtree_array(ett, array_length(ett));

    /
    jxta_module = prefs_register_protocol(proto_jxta, proto_reg_handoff_jxta);

    prefs_register_bool_preference(jxta_module, "desegment",
                                   "Reassemble JXTA messages spanning multiple UDP/TCP/HTTP segments",
                                   "Whether the JXTA dissector should reassemble messages spanning multiple UDP/TCP segments."
                                   " To use this option you must also enable \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings "
                                   " and enable \"Reassemble fragmented IP datagrams\" in the IP protocol settings.",
                                   &gDESEGMENT);

    prefs_register_bool_preference(jxta_module, "udp.heuristic", "Try to discover JXTA in UDP datagrams",
                                   "Enable to inspect UDP datagrams for JXTA messages.", &gUDP_HEUR);

    prefs_register_bool_preference(jxta_module, "tcp.heuristic", "Try to discover JXTA in TCP connections",
                                   "Enable to inspect TCP connections for JXTA conversations.", &gTCP_HEUR);
}
