void proto_reg_handoff_waveagent(void)
{
    static gboolean inited = FALSE;

    if (!inited) {

        /
        new_create_dissector_handle(dissect_waveagent,
                                    proto_waveagent);
        heur_dissector_add("udp", dissect_waveagent, proto_waveagent);

        inited = TRUE;
    }
}
