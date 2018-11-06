static guint32 dissect_wa_header(guint32 starting_offset, proto_item *parent_tree, tvbuff_t *tvb, guint8 version)
{
    guint32 wa_payload_offset;

    proto_tree_add_item(parent_tree,
        hf_waveagent_controlword, tvb, 30+starting_offset, 2, ENC_BIG_ENDIAN); 

    proto_tree_add_item(parent_tree,
        hf_waveagent_payloadlen, tvb, 20+starting_offset, 4, ENC_BIG_ENDIAN);

    proto_tree_add_item(parent_tree,
        hf_waveagent_transnum, tvb, 24+starting_offset, 4, ENC_BIG_ENDIAN);

    proto_tree_add_item(parent_tree,
        hf_waveagent_rtoken, tvb, 32+starting_offset, 4, ENC_BIG_ENDIAN);

    proto_tree_add_item(parent_tree,
        hf_waveagent_flowid, tvb, 36+starting_offset, 4, ENC_BIG_ENDIAN);

    if (version >= 3) {
        proto_tree_add_item(parent_tree,
            hf_waveagent_capstatus, tvb, 40+starting_offset, 4, ENC_BIG_ENDIAN);

        proto_tree_add_item(parent_tree,
            hf_waveagent_protocolversion, tvb, 40+starting_offset, 1, ENC_BIG_ENDIAN);

        wa_payload_offset = WA_V3_PAYLOAD_OFFSET + starting_offset;
    }
    else {
        wa_payload_offset = WA_V2_PAYLOAD_OFFSET + starting_offset;
    }

    proto_tree_add_item(parent_tree,
        hf_waveagent_sigsequencenum, tvb, 4+starting_offset, 1, ENC_BIG_ENDIAN);

    return wa_payload_offset;
}
