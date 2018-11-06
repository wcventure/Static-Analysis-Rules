void
proto_register_ssl(void)
{

    /
    static hf_register_info hf[] = {
        { &hf_ssl_record,
          { "Record Layer", "ssl.record",
            FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_record_content_type,
          { "Content Type", "ssl.record.content_type",
            FT_UINT8, BASE_DEC, VALS(ssl_31_content_type), 0x0,
            NULL, HFILL}
        },
        { &hf_ssl2_msg_type,
          { "Handshake Message Type", "ssl.handshake.type",
            FT_UINT8, BASE_DEC, VALS(ssl_20_msg_types), 0x0,
            "SSLv2 handshake message type", HFILL}
        },
        { &hf_pct_msg_type,
          { "Handshake Message Type", "ssl.pct_handshake.type",
            FT_UINT8, BASE_DEC, VALS(pct_msg_types), 0x0,
            "PCT handshake message type", HFILL}
        },
        { &hf_ssl_record_version,
          { "Version", "ssl.record.version",
            FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
            "Record layer version", HFILL }
        },
        { &hf_ssl_record_length,
          { "Length", "ssl.record.length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of SSL record data", HFILL }
        },
        { &hf_ssl_record_appdata,
          { "Encrypted Application Data", "ssl.app_data",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            "Payload is encrypted application data", HFILL }
        },

        { &hf_ssl2_record,
          { "SSLv2/PCT Record Header", "ssl.record",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "SSLv2/PCT record data", HFILL }
        },
        { &hf_ssl2_record_is_escape,
          { "Is Escape", "ssl.record.is_escape",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Indicates a security escape", HFILL}
        },
        { &hf_ssl2_record_padding_length,
          { "Padding Length", "ssl.record.padding_length",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "Length of padding at end of record", HFILL }
        },
        { &hf_ssl_change_cipher_spec,
          { "Change Cipher Spec Message", "ssl.change_cipher_spec",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Signals a change in cipher specifications", HFILL }
        },
        { &hf_ssl_alert_message,
          { "Alert Message", "ssl.alert_message",
            FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_alert_message_level,
          { "Level", "ssl.alert_message.level",
            FT_UINT8, BASE_DEC, VALS(ssl_31_alert_level), 0x0,
            "Alert message level", HFILL }
        },
        { &hf_ssl_alert_message_description,
          { "Description", "ssl.alert_message.desc",
            FT_UINT8, BASE_DEC, VALS(ssl_31_alert_description), 0x0,
            "Alert message description", HFILL }
        },
        { &hf_ssl_handshake_protocol,
          { "Handshake Protocol", "ssl.handshake",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Handshake protocol message", HFILL}
        },
        { &hf_ssl_handshake_type,
          { "Handshake Type", "ssl.handshake.type",
            FT_UINT8, BASE_DEC, VALS(ssl_31_handshake_type), 0x0,
            "Type of handshake message", HFILL}
        },
        { &hf_ssl_handshake_length,
          { "Length", "ssl.handshake.length",
            FT_UINT24, BASE_DEC, NULL, 0x0,
            "Length of handshake message", HFILL }
        },
        { &hf_ssl_handshake_client_version,
          { "Version", "ssl.handshake.version",
            FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
            "Maximum version supported by client", HFILL }
        },
        { &hf_ssl_handshake_server_version,
          { "Version", "ssl.handshake.version",
            FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
            "Version selected by server", HFILL }
        },
        { &hf_ssl_handshake_random_time,
          { "gmt_unix_time", "ssl.handshake.random_time",
            FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL, NULL, 0x0,
            "Unix time field of random structure", HFILL }
        },
        { &hf_ssl_handshake_random_bytes,
          { "random_bytes", "ssl.handshake.random_bytes",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            "Random challenge used to authenticate server", HFILL }
        },
        { &hf_ssl_handshake_cipher_suites_len,
          { "Cipher Suites Length", "ssl.handshake.cipher_suites_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of cipher suites field", HFILL }
        },
        { &hf_ssl_handshake_cipher_suites,
          { "Cipher Suites", "ssl.handshake.ciphersuites",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of cipher suites supported by client", HFILL }
        },
        { &hf_ssl_handshake_cipher_suite,
          { "Cipher Suite", "ssl.handshake.ciphersuite",
            FT_UINT16, BASE_HEX|BASE_EXT_STRING, &ssl_31_ciphersuite_ext, 0x0,
            NULL, HFILL }
        },
        { &hf_ssl2_handshake_cipher_spec,
          { "Cipher Spec", "ssl.handshake.cipherspec",
            FT_UINT24, BASE_HEX|BASE_EXT_STRING, &ssl_20_cipher_suites_ext, 0x0,
            "Cipher specification", HFILL }
        },
        { &hf_ssl_handshake_session_id,
          { "Session ID", "ssl.handshake.session_id",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            "Identifies the SSL session, allowing later resumption", HFILL }
        },
        { &hf_ssl_handshake_comp_methods_len,
          { "Compression Methods Length", "ssl.handshake.comp_methods_length",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "Length of compression methods field", HFILL }
        },
        { &hf_ssl_handshake_comp_methods,
          { "Compression Methods", "ssl.handshake.comp_methods",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of compression methods supported by client", HFILL }
        },
        { &hf_ssl_handshake_comp_method,
          { "Compression Method", "ssl.handshake.comp_method",
            FT_UINT8, BASE_DEC, VALS(ssl_31_compression_method), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_extensions_len,
          { "Extensions Length", "ssl.handshake.extensions_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of hello extensions", HFILL }
        },
        { &hf_ssl_handshake_extension_type,
          { "Type", "ssl.handshake.extension.type",
            FT_UINT16, BASE_HEX, VALS(tls_hello_extension_types), 0x0,
            "Hello extension type", HFILL }
        },
        { &hf_ssl_handshake_extension_len,
          { "Length", "ssl.handshake.extension.len",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of a hello extension", HFILL }
        },
        { &hf_ssl_handshake_extension_data,
          { "Data", "ssl.handshake.extension.data",
            FT_BYTES, BASE_NONE, NULL, 0x0,
            "Hello Extension data", HFILL }
        },
        { &hf_ssl_handshake_extension_elliptic_curves_len,
          { "Elliptic Curves Length", "ssl.handshake.extensions_elliptic_curves_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of elliptic curves field", HFILL }
        },
        { &hf_ssl_handshake_extension_elliptic_curves,
          { "Elliptic Curves List", "ssl.handshake.extensions_elliptic_curves",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of elliptic curves supported", HFILL }
        },
        { &hf_ssl_handshake_extension_elliptic_curve,
          { "Elliptic curve", "ssl.handshake.extensions_elliptic_curve",
            FT_UINT16, BASE_HEX, VALS(ssl_extension_curves), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_extension_ec_point_formats_len,
          { "EC point formats Length", "ssl.handshake.extensions_ec_point_formats_length",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "Length of elliptic curves point formats field", HFILL }
        },
        { &hf_ssl_handshake_extension_ec_point_format,
          { "EC point format", "ssl.handshake.extensions_ec_point_format",
            FT_UINT8, BASE_DEC, VALS(ssl_extension_ec_point_formats), 0x0,
            "Elliptic curves point format", HFILL }
        },
        { &hf_ssl_handshake_certificates_len,
          { "Certificates Length", "ssl.handshake.certificates_length",
            FT_UINT24, BASE_DEC, NULL, 0x0,
            "Length of certificates field", HFILL }
        },
        { &hf_ssl_handshake_certificates,
          { "Certificates", "ssl.handshake.certificates",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of certificates", HFILL }
        },
        { &hf_ssl_handshake_certificate,
          { "Certificate", "ssl.handshake.certificate",
            FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_certificate_len,
          { "Certificate Length", "ssl.handshake.certificate_length",
            FT_UINT24, BASE_DEC, NULL, 0x0,
            "Length of certificate", HFILL }
        },
        { &hf_ssl_handshake_cert_types_count,
          { "Certificate types count", "ssl.handshake.cert_types_count",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "Count of certificate types", HFILL }
        },
        { &hf_ssl_handshake_cert_types,
          { "Certificate types", "ssl.handshake.cert_types",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of certificate types", HFILL }
        },
        { &hf_ssl_handshake_cert_type,
          { "Certificate type", "ssl.handshake.cert_type",
            FT_UINT8, BASE_DEC, VALS(ssl_31_client_certificate_type), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_sig_hash_alg_len,
          { "Signature Hash Algorithms Length", "ssl.handshake.sig_hash_alg_len",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of Signature Hash Algorithms", HFILL }
        },
        { &hf_ssl_handshake_sig_hash_algs,
          { "Signature Hash Algorithms", "ssl.handshake.sig_hash_algs",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of Signature Hash Algorithms", HFILL }
        },
        { &hf_ssl_handshake_sig_hash_alg,
          { "Signature Hash Algorithm", "ssl.handshake.sig_hash_alg",
            FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_sig_hash_hash,
          { "Signature Hash Algorithm Hash", "ssl.handshake.sig_hash_hash",
            FT_UINT8, BASE_DEC, VALS(tls_hash_algorithm), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_sig_hash_sig,
          { "Signature Hash Algorithm Signature", "ssl.handshake.sig_hash_sig",
            FT_UINT8, BASE_DEC, VALS(tls_signature_algorithm), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_cert_status,
          { "Certificate Status", "ssl.handshake.cert_status",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Certificate Status Data", HFILL }
        },
        { &hf_ssl_handshake_cert_status_type,
          { "Certificate Status Type", "ssl.handshake.cert_status_type",
            FT_UINT8, BASE_DEC, VALS(tls_cert_status_type), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl_handshake_cert_status_len,
          { "Certificate Status Length", "ssl.handshake.cert_status_len",
            FT_UINT24, BASE_DEC, NULL, 0x0,
            "Length of certificate status", HFILL }
        },
        { &hf_ssl_handshake_finished,
          { "Verify Data", "ssl.handshake.verify_data",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Opaque verification data", HFILL }
        },
        { &hf_ssl_handshake_md5_hash,
          { "MD5 Hash", "ssl.handshake.md5_hash",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Hash of messages, master_secret, etc.", HFILL }
        },
        { &hf_ssl_handshake_sha_hash,
          { "SHA-1 Hash", "ssl.handshake.sha_hash",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Hash of messages, master_secret, etc.", HFILL }
        },
        { &hf_ssl_handshake_session_id_len,
          { "Session ID Length", "ssl.handshake.session_id_length",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "Length of session ID field", HFILL }
        },
        { &hf_ssl_handshake_dnames_len,
          { "Distinguished Names Length", "ssl.handshake.dnames_len",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of list of CAs that server trusts", HFILL }
        },
        { &hf_ssl_handshake_dnames,
          { "Distinguished Names", "ssl.handshake.dnames",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "List of CAs that server trusts", HFILL }
        },
        { &hf_ssl_handshake_dname_len,
          { "Distinguished Name Length", "ssl.handshake.dname_len",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of distinguished name", HFILL }
        },
        { &hf_ssl_handshake_dname,
          { "Distinguished Name", "ssl.handshake.dname",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Distinguished name of a CA that server trusts", HFILL }
        },
        { &hf_ssl2_handshake_challenge,
          { "Challenge", "ssl.handshake.challenge",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Challenge data used to authenticate server", HFILL }
        },
        { &hf_ssl2_handshake_cipher_spec_len,
          { "Cipher Spec Length", "ssl.handshake.cipher_spec_len",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of cipher specs field", HFILL }
        },
        { &hf_ssl2_handshake_session_id_len,
          { "Session ID Length", "ssl.handshake.session_id_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of session ID field", HFILL }
        },
        { &hf_ssl2_handshake_challenge_len,
          { "Challenge Length", "ssl.handshake.challenge_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of challenge field", HFILL }
        },
        { &hf_ssl2_handshake_clear_key_len,
          { "Clear Key Data Length", "ssl.handshake.clear_key_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of clear key data", HFILL }
        },
        { &hf_ssl2_handshake_enc_key_len,
          { "Encrypted Key Data Length", "ssl.handshake.encrypted_key_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of encrypted key data", HFILL }
        },
        { &hf_ssl2_handshake_key_arg_len,
          { "Key Argument Length", "ssl.handshake.key_arg_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of key argument", HFILL }
        },
        { &hf_ssl2_handshake_clear_key,
          { "Clear Key Data", "ssl.handshake.clear_key_data",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Clear portion of MASTER-KEY", HFILL }
        },
        { &hf_ssl2_handshake_enc_key,
          { "Encrypted Key", "ssl.handshake.encrypted_key",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Secret portion of MASTER-KEY encrypted to server", HFILL }
        },
        { &hf_ssl2_handshake_key_arg,
          { "Key Argument", "ssl.handshake.key_arg",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Key Argument (e.g., Initialization Vector)", HFILL }
        },
        { &hf_ssl2_handshake_session_id_hit,
          { "Session ID Hit", "ssl.handshake.session_id_hit",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Did the server find the client's Session ID?", HFILL }
        },
        { &hf_ssl2_handshake_cert_type,
          { "Certificate Type", "ssl.handshake.cert_type",
            FT_UINT8, BASE_DEC, VALS(ssl_20_certificate_type), 0x0,
            NULL, HFILL }
        },
        { &hf_ssl2_handshake_connection_id_len,
          { "Connection ID Length", "ssl.handshake.connection_id_length",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            "Length of connection ID", HFILL }
        },
        { &hf_ssl2_handshake_connection_id,
          { "Connection ID", "ssl.handshake.connection_id",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "Server's challenge to client", HFILL }
        },
        { &hf_pct_handshake_cipher_spec,
          { "Cipher Spec", "pct.handshake.cipherspec",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "PCT Cipher specification", HFILL }
        },
        { &hf_pct_handshake_cipher,
          { "Cipher", "pct.handshake.cipher",
            FT_UINT16, BASE_HEX, VALS(pct_cipher_type), 0x0,
            "PCT Ciper", HFILL }
        },
        { &hf_pct_handshake_hash_spec,
          { "Hash Spec", "pct.handshake.hashspec",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "PCT Hash specification", HFILL }
        },
        { &hf_pct_handshake_hash,
          { "Hash", "pct.handshake.hash",
            FT_UINT16, BASE_HEX, VALS(pct_hash_type), 0x0,
            "PCT Hash", HFILL }
        },
        { &hf_pct_handshake_cert_spec,
          { "Cert Spec", "pct.handshake.certspec",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "PCT Certificate specification", HFILL }
        },
        { &hf_pct_handshake_cert,
          { "Cert", "pct.handshake.cert",
            FT_UINT16, BASE_HEX, VALS(pct_cert_type), 0x0,
            "PCT Certificate", HFILL }
        },
        { &hf_pct_handshake_exch_spec,
          { "Exchange Spec", "pct.handshake.exchspec",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "PCT Exchange specification", HFILL }
        },
        { &hf_pct_handshake_exch,
          { "Exchange", "pct.handshake.exch",
            FT_UINT16, BASE_HEX, VALS(pct_exch_type), 0x0,
            "PCT Exchange", HFILL }
        },
        { &hf_pct_handshake_sig,
          { "Sig Spec", "pct.handshake.sig",
            FT_UINT16, BASE_HEX, VALS(pct_sig_type), 0x0,
            "PCT Signature", HFILL }
        },
        { &hf_pct_msg_error_type,
          { "PCT Error Code", "pct.msg_error_code",
            FT_UINT16, BASE_HEX, VALS(pct_error_code), 0x0,
            NULL, HFILL }
        },
        { &hf_pct_handshake_server_cert,
          { "Server Cert", "pct.handshake.server_cert",
            FT_NONE, BASE_NONE, NULL , 0x0,
            "PCT Server Certificate", HFILL }
        },
        { &hf_ssl_segment_overlap,
          { "Segment overlap", "ssl.segment.overlap",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Segment overlaps with other segments", HFILL }},

        { &hf_ssl_segment_overlap_conflict,
          { "Conflicting data in segment overlap", "ssl.segment.overlap.conflict",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Overlapping segments contained conflicting data", HFILL }},

        { &hf_ssl_segment_multiple_tails,
          { "Multiple tail segments found", "ssl.segment.multipletails",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Several tails were found when reassembling the pdu", HFILL }},

        { &hf_ssl_segment_too_long_fragment,
          { "Segment too long", "ssl.segment.toolongfragment",
            FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Segment contained data past end of the pdu", HFILL }},

        { &hf_ssl_segment_error,
          { "Reassembling error", "ssl.segment.error",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "Reassembling error due to illegal segments", HFILL }},

        { &hf_ssl_segment_count,
          { "Segment count", "ssl.segment.count",
            FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ssl_segment,
          { "SSL Segment", "ssl.segment",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ssl_segments,
          { "Reassembled SSL Segments", "ssl.segments",
            FT_NONE, BASE_NONE, NULL, 0x0,
            "SSL Segments", HFILL }},

        { &hf_ssl_reassembled_in,
          { "Reassembled PDU in frame", "ssl.reassembled_in",
            FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "The PDU that doesn't end in this segment is reassembled in this frame", HFILL }},

        { &hf_ssl_reassembled_length,
          { "Reassembled PDU length", "ssl.reassembled.length",
            FT_UINT32, BASE_DEC, NULL, 0x0,
            "The total length of the reassembled payload", HFILL }},
    };

    /
    static gint *ett[] = {
        &ett_ssl,
        &ett_ssl_record,
        &ett_ssl_alert,
        &ett_ssl_handshake,
        &ett_ssl_cipher_suites,
        &ett_ssl_comp_methods,
        &ett_ssl_extension,
        &ett_ssl_extension_curves,
        &ett_ssl_extension_curves_point_formats,
        &ett_ssl_certs,
        &ett_ssl_cert_types,
        &ett_ssl_sig_hash_algs,
        &ett_ssl_sig_hash_alg,
        &ett_ssl_dnames,
        &ett_ssl_random,
        &ett_ssl_cert_status,
        &ett_ssl_ocsp_resp,
        &ett_pct_cipher_suites,
        &ett_pct_hash_suites,
        &ett_pct_cert_suites,
        &ett_pct_exch_suites,
        &ett_ssl_segments,
        &ett_ssl_segment
    };

    /
    proto_ssl = proto_register_protocol("Secure Sockets Layer",
                                        "SSL", "ssl");

    /
    proto_register_field_array(proto_ssl, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    {
        module_t *ssl_module = prefs_register_protocol(proto_ssl, proto_reg_handoff_ssl);
        prefs_register_bool_preference(ssl_module,
             "desegment_ssl_records",
             "Reassemble SSL records spanning multiple TCP segments",
             "Whether the SSL dissector should reassemble SSL records spanning multiple TCP segments. "
             "To use this option, you must also enable \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings.",
             &ssl_desegment);
        prefs_register_bool_preference(ssl_module,
             "desegment_ssl_application_data",
             "Reassemble SSL Application Data spanning multiple SSL records",
             "Whether the SSL dissector should reassemble SSL Application Data spanning multiple SSL records. ",
             &ssl_desegment_app_data);
#ifdef HAVE_LIBGNUTLS
        prefs_register_string_preference(ssl_module, "keys_list", "RSA keys list",
             "Semicolon-separated list of private RSA keys used for SSL decryption; "
             "each list entry must be in the form of <ip>,<port>,<protocol>,<key_file_name>. "
             "<key_file_name> is the local file name of the RSA private key used by the specified server "
             "(or name of the file containing such a list)",
             (const gchar **)&ssl_keys_list);
        prefs_register_string_preference(ssl_module, "debug_file", "SSL debug file",
             "Redirect SSL debug to file name; leave empty to disable debugging, "
             "or use \"" SSL_DEBUG_USE_STDERR "\" to redirect output to stderr\n",
             (const gchar **)&ssl_debug_file_name);
        prefs_register_string_preference(ssl_module, "psk", "Pre-Shared-Key",
             "Pre-Shared-Key as HEX string, should be 0 to 16 bytes",
             (const gchar **)&ssl_psk);
#endif
    }

    register_dissector("ssl", dissect_ssl, proto_ssl);
    ssl_handle = find_dissector("ssl");

    ssl_associations = g_tree_new(ssl_association_cmp);

    register_init_routine(ssl_init);
    ssl_lib_init();
    ssl_tap = register_tap("ssl");
    ssl_debug_printf("proto_register_ssl: registered tap %s:%d\n",
        "ssl", ssl_tap);
}
