static int dissect_waveagent(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

/
    proto_item *ti, *rmi;
    proto_tree *waveagent_tree, *relay_message_tree, *payload_tree;
    guint8 signature_start, signature_end;
    guint8 version;
    guint32 magic_number;
    guint32 control_word, paylen;
    guint32 wa_payload_offset;

/
    /
    if (tvb_length(tvb) < 52 )
        return 0;

    signature_start = tvb_get_guint8(tvb, 0);
    signature_end = tvb_get_guint8(tvb, 15);
    version = ((tvb_get_ntohl(tvb, 16) & 0xF0000000) >> 28 == 1) ? 3 : 2;       / 
    magic_number = tvb_get_ntohl(tvb, 16) & 0x0FFFFFFF;  /

    /
    if ( (signature_start != 0xcc && signature_start !=0xdd) ||
         signature_end != 0xE2 || magic_number != 0x0F87C3A5 ) 
        /
        return 0;

/
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "WA");

    if (check_col(pinfo->cinfo, COL_INFO))
        col_clear(pinfo->cinfo, COL_INFO);


/

    control_word = tvb_get_ntohl(tvb, 28);
    paylen = tvb_get_ntohl(tvb, 20);
        
    col_add_fstr(pinfo->cinfo, COL_INFO, "%s (0x%x)", 
        val_to_str_const(control_word, control_words, "Unknown"), control_word);

    if (tree) {
/
        ti = proto_tree_add_protocol_format(tree, proto_waveagent, tvb, 0, -1, 
                        "WaveAgent, %s (0x%x), Payload Length %u Bytes",
                        val_to_str_const(control_word, control_words, "Unknown"), control_word, paylen);

        waveagent_tree = proto_item_add_subtree(ti, ett_waveagent);

        wa_payload_offset = dissect_wa_header(0, waveagent_tree, tvb, version);

        payload_tree = waveagent_tree;

        /
        if (control_word == 0x3e)
        {
            proto_tree_add_item(waveagent_tree,
                hf_waveagent_relaydestid, tvb, wa_payload_offset, 4, ENC_BIG_ENDIAN);
            proto_tree_add_item(waveagent_tree,
                hf_waveagent_relaysrcid, tvb, wa_payload_offset+4, 4, ENC_BIG_ENDIAN);

            /
            control_word = tvb_get_ntohl(tvb, wa_payload_offset+12+28);
                rmi = proto_tree_add_none_format(waveagent_tree, hf_waveagent_relaymessagest, tvb, wa_payload_offset+12+28, 0,
                                    "Relayed WaveAgent Message, %s (0x%x)",
                                                    val_to_str_const(control_word, control_words, "Unknown"), control_word);

            relay_message_tree = proto_item_add_subtree(rmi, ett_relaymessage);

            wa_payload_offset = dissect_wa_header(wa_payload_offset+12, relay_message_tree, tvb, version);
            payload_tree = relay_message_tree;
        }

        dissect_wa_payload(wa_payload_offset, payload_tree, tvb, control_word, version);
    }

/
    return tvb_length(tvb);
}
