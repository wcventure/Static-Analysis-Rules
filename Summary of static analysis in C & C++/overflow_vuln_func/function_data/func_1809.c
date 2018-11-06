void
proto_register_nmas(void)
{
    static hf_register_info hf_nmas[] = {
        { &hf_func,
        { "Function",		"nmas.func", FT_UINT8, BASE_HEX, NULL, 0x0,
            "Function", HFILL }},

        { &hf_subfunc,
        { "Subfunction",		"nmas.subfunc", FT_UINT8, BASE_HEX, NULL, 0x0,
            "Subfunction", HFILL }},

        { &hf_ping_version,
        { "Ping Version",		"nmas.ping_version", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Ping Version", HFILL }},

        { &hf_ping_flags,
        { "Flags",		"nmas.ping_flags", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Flags", HFILL }},

        { &hf_frag_handle,
        { "Fragment Handle",        "nmas.frag_handle", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Fragment Handle", HFILL }},

        { &hf_length,
        { "Length",        "nmas.length", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Length", HFILL }},

        { &hf_subverb,
        { "Sub Verb",    "nmas.subverb",
          FT_UINT32,    BASE_HEX,   VALS(nmas_subverb_enum),   0x0,
          "Sub Verb", HFILL }},

        { &hf_tree,
        { "Tree",    "nmas.tree",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Tree", HFILL }},

        { &hf_user,
        { "User",    "nmas.user",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "User", HFILL }},

        { &hf_nmas_version,
        { "NMAS Protocol Version",        "nmas.version", FT_UINT32, BASE_HEX, NULL, 0x0,
            "NMAS Protocol Version", HFILL }},

        { &hf_msg_version,
        { "Message Version",        "nmas.msg_version", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Message Version", HFILL }},

        { &hf_session_ident,
        { "Session Identifier",        "nmas.session_ident", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Session Identifier", HFILL }},

        { &hf_msg_verb,
        { "Message Verb",        "nmas.msg_verb", FT_UINT8, BASE_HEX, VALS(nmas_msgverb_enum), 0x0,
            "Message Verb", HFILL }},
        
        { &hf_attribute,
        { "Attribute Type",        "nmas.attribute", FT_UINT32, BASE_DEC, VALS(nmas_attribute_enum), 0x0,
            "Attribute Type", HFILL }},

        { &hf_clearence,
        { "Requested Clearence",    "nmas.clearence",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Requested Clearence", HFILL }},

        { &hf_login_sequence,
        { "Requested Login Sequence",    "nmas.login_seq",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Requested Login Sequence", HFILL }},

        { &hf_opaque,
        { "Opaque Data",    "nmas.opaque",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Opaque Data", HFILL }},

        { &hf_data,
        { "Data",    "nmas.data",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Data", HFILL }},
        
        { &hf_return_code,
        { "Return Code",        "nmas.return_code", FT_UINT32, BASE_HEX, VALS(nmas_errors_enum), 0x0,
            "Return Code", HFILL }},
       
        { &hf_lsm_verb,
        { "Login Store Message Verb",        "nmas.lsm_verb", FT_UINT8, BASE_HEX, VALS(nmas_lsmverb_enum), 0x0,
            "Login Store Message Verb", HFILL }},

        { &hf_squeue_bytes,
        { "Server Queue Number of Bytes",        "nmas.squeue_bytes", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Server Queue Number of Bytes", HFILL }},

        { &hf_cqueue_bytes,
        { "Client Queue Number of Bytes",        "nmas.cqueue_bytes", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Client Queue Number of Bytes", HFILL }},

        { &hf_num_creds,
        { "Number of Credentials",        "nmas.num_creds", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Number of Credentials", HFILL }},

        { &hf_cred_type,
        { "Credential Type",        "nmas.cred_type", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Credential Type", HFILL }},
        
        { &hf_login_state,
        { "Login State",        "nmas.login_state", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Login State", HFILL }},

        { &hf_enc_cred,
        { "Encrypted Credential",    "nmas.enc_cred",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Encrypted Credential", HFILL }},

        { &hf_enc_data,
        { "Encrypted Data",    "nmas.enc_data",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Encrypted Data", HFILL }},
    
        { &hf_reply_buffer_size,
        { "Reply Buffer Size",        "nmas.buf_size", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Reply Buffer Size", HFILL }},
    
        { &hf_encrypt_error,
        { "Payload Error",        "nmas.encrypt_error", FT_UINT32, BASE_HEX, VALS(nmas_errors_enum), 0x0,
            "Payload/Encryption Return Code", HFILL }},
        
    };

    static gint *ett[] = {
        &ett_nmas,
    };
    
    proto_nmas = proto_register_protocol("Novell Modular Authentication Service", "NMAS", "nmas");
    proto_register_field_array(proto_nmas, hf_nmas, array_length(hf_nmas));
    proto_register_subtree_array(ett, array_length(ett));
}
