void
proto_register_dtls(void)
{

  /
  static hf_register_info hf[] = {
    { &hf_dtls_record,
      { "Record Layer", "dtls.record",
        FT_NONE, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_record_content_type,
      { "Content Type", "dtls.record.content_type",
        FT_UINT8, BASE_DEC, VALS(ssl_31_content_type), 0x0,
        NULL, HFILL}
    },
    { &hf_dtls_record_version,
      { "Version", "dtls.record.version",
        FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
        "Record layer version.", HFILL }
    },
    { &hf_dtls_record_epoch,
      { "Epoch", "dtls.record.epoch",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_record_sequence_number,
      { "Sequence Number", "dtls.record.sequence_number",
        FT_DOUBLE, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_record_length,
      { "Length", "dtls.record.length",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of DTLS record data", HFILL }
    },
    { &hf_dtls_record_appdata,
      { "Encrypted Application Data", "dtls.app_data",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        "Payload is encrypted application data", HFILL }
    },
    { &hf_dtls_change_cipher_spec,
      { "Change Cipher Spec Message", "dtls.change_cipher_spec",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Signals a change in cipher specifications", HFILL }
    },
    { & hf_dtls_alert_message,
      { "Alert Message", "dtls.alert_message",
        FT_NONE, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },
    { & hf_dtls_alert_message_level,
      { "Level", "dtls.alert_message.level",
        FT_UINT8, BASE_DEC, VALS(ssl_31_alert_level), 0x0,
        "Alert message level", HFILL }
    },
    { &hf_dtls_alert_message_description,
      { "Description", "dtls.alert_message.desc",
        FT_UINT8, BASE_DEC, VALS(ssl_31_alert_description), 0x0,
        "Alert message description", HFILL }
    },
    { &hf_dtls_handshake_protocol,
      { "Handshake Protocol", "dtls.handshake",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Handshake protocol message", HFILL}
    },
    { &hf_dtls_handshake_type,
      { "Handshake Type", "dtls.handshake.type",
        FT_UINT8, BASE_DEC, VALS(ssl_31_handshake_type), 0x0,
        "Type of handshake message", HFILL}
    },
    { &hf_dtls_handshake_length,
      { "Length", "dtls.handshake.length",
        FT_UINT24, BASE_DEC, NULL, 0x0,
        "Length of handshake message", HFILL }
    },
    { &hf_dtls_handshake_message_seq,
      { "Message Sequence", "dtls.handshake.message_seq",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Message sequence of handshake message", HFILL }
    },
    { &hf_dtls_handshake_fragment_offset,
      { "Fragment Offset", "dtls.handshake.fragment_offset",
        FT_UINT24, BASE_DEC, NULL, 0x0,
        "Fragment offset of handshake message", HFILL }
    },
    { &hf_dtls_handshake_fragment_length,
      { "Fragment Length", "dtls.handshake.fragment_length",
        FT_UINT24, BASE_DEC, NULL, 0x0,
        "Fragment length of handshake message", HFILL }
    },
    { &hf_dtls_handshake_client_version,
      { "Version", "dtls.handshake.version",
        FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
        "Maximum version supported by client", HFILL }
    },
    { &hf_dtls_handshake_server_version,
      { "Version", "dtls.handshake.version",
        FT_UINT16, BASE_HEX, VALS(ssl_versions), 0x0,
        "Version selected by server", HFILL }
    },
    { &hf_dtls_handshake_random_time,
      { "Random.gmt_unix_time", "dtls.handshake.random_time",
        FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL, NULL, 0x0,
        "Unix time field of random structure", HFILL }
    },
    { &hf_dtls_handshake_random_bytes,
      { "Random.bytes", "dtls.handshake.random",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Random challenge used to authenticate server", HFILL }
    },
    { &hf_dtls_handshake_cipher_suites_len,
      { "Cipher Suites Length", "dtls.handshake.cipher_suites_length",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of cipher suites field", HFILL }
    },
    { &hf_dtls_handshake_cipher_suites,
      { "Cipher Suites", "dtls.handshake.ciphersuites",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "List of cipher suites supported by client", HFILL }
    },
    { &hf_dtls_handshake_cipher_suite,
      { "Cipher Suite", "dtls.handshake.ciphersuite",
        FT_UINT16, BASE_HEX|BASE_EXT_STRING, &ssl_31_ciphersuite_ext, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_handshake_cookie_len,
      { "Cookie Length", "dtls.handshake.cookie_length",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Length of the cookie field", HFILL }
    },
    { &hf_dtls_handshake_cookie,
      { "Cookie", "dtls.handshake.cookie",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_handshake_session_id,
      { "Session ID", "dtls.handshake.session_id",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        "Identifies the DTLS session, allowing later resumption", HFILL }
    },
    { &hf_dtls_handshake_comp_methods_len,
      { "Compression Methods Length", "dtls.handshake.comp_methods_length",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Length of compression methods field", HFILL }
    },
    { &hf_dtls_handshake_comp_methods,
      { "Compression Methods", "dtls.handshake.comp_methods",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "List of compression methods supported by client", HFILL }
    },
    { &hf_dtls_handshake_comp_method,
      { "Compression Method", "dtls.handshake.comp_method",
        FT_UINT8, BASE_DEC, VALS(ssl_31_compression_method), 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_handshake_extensions_len,
      { "Extensions Length", "dtls.handshake.extensions_length",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of hello extensions", HFILL }
    },
    { &hf_dtls_handshake_extension_type,
      { "Type", "dtls.handshake.extension.type",
        FT_UINT16, BASE_HEX, VALS(tls_hello_extension_types), 0x0,
        "Hello extension type", HFILL }
    },
    { &hf_dtls_handshake_extension_len,
      { "Length", "dtls.handshake.extension.len",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of a hello extension", HFILL }
    },
    { &hf_dtls_handshake_extension_data,
      { "Data", "dtls.handshake.extension.data",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        "Hello Extension data", HFILL }
    },
    { &hf_dtls_handshake_certificates_len,
      { "Certificates Length", "dtls.handshake.certificates_length",
        FT_UINT24, BASE_DEC, NULL, 0x0,
        "Length of certificates field", HFILL }
    },
    { &hf_dtls_handshake_certificates,
      { "Certificates", "dtls.handshake.certificates",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "List of certificates", HFILL }
    },
    { &hf_dtls_handshake_certificate,
      { "Certificate", "dtls.handshake.certificate",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_handshake_certificate_len,
      { "Certificate Length", "dtls.handshake.certificate_length",
        FT_UINT24, BASE_DEC, NULL, 0x0,
        "Length of certificate", HFILL }
    },
    { &hf_dtls_handshake_cert_types_count,
      { "Certificate types count", "dtls.handshake.cert_types_count",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Count of certificate types", HFILL }
    },
    { &hf_dtls_handshake_cert_types,
      { "Certificate types", "dtls.handshake.cert_types",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "List of certificate types", HFILL }
    },
    { &hf_dtls_handshake_cert_type,
      { "Certificate type", "dtls.handshake.cert_type",
        FT_UINT8, BASE_DEC, VALS(ssl_31_client_certificate_type), 0x0,
        NULL, HFILL }
    },
    { &hf_dtls_handshake_finished,
      { "Verify Data", "dtls.handshake.verify_data",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Opaque verification data", HFILL }
    },
    { &hf_dtls_handshake_md5_hash,
      { "MD5 Hash", "dtls.handshake.md5_hash",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Hash of messages, master_secret, etc.", HFILL }
    },
    { &hf_dtls_handshake_sha_hash,
      { "SHA-1 Hash", "dtls.handshake.sha_hash",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "Hash of messages, master_secret, etc.", HFILL }
    },
    { &hf_dtls_handshake_session_id_len,
      { "Session ID Length", "dtls.handshake.session_id_length",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Length of session ID field", HFILL }
    },
    { &hf_dtls_handshake_dnames_len,
      { "Distinguished Names Length", "dtls.handshake.dnames_len",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of list of CAs that server trusts", HFILL }
    },
    { &hf_dtls_handshake_dnames,
      { "Distinguished Names", "dtls.handshake.dnames",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "List of CAs that server trusts", HFILL }
    },
    { &hf_dtls_handshake_dname_len,
      { "Distinguished Name Length", "dtls.handshake.dname_len",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Length of distinguished name", HFILL }
    },
    { &hf_dtls_handshake_dname,
      { "Distinguished Name", "dtls.handshake.dname",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        "Distinguished name of a CA that server trusts", HFILL }
    },
    { &hf_dtls_fragments,
      { "Message fragments", "dtls.fragments",
        FT_NONE, BASE_NONE, NULL, 0x00, NULL, HFILL }
    },
    { &hf_dtls_fragment,
      { "Message fragment", "dtls.fragment",
        FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL }
    },
    { &hf_dtls_fragment_overlap,
      { "Message fragment overlap", "dtls.fragment.overlap",
        FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL }
    },
    { &hf_dtls_fragment_overlap_conflicts,
      { "Message fragment overlapping with conflicting data",
        "dtls.fragment.overlap.conflicts",
       FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL }
    },
    { &hf_dtls_fragment_multiple_tails,
      { "Message has multiple tail fragments",
        "dtls.fragment.multiple_tails",
        FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL }
    },
    { &hf_dtls_fragment_too_long_fragment,
      { "Message fragment too long", "dtls.fragment.too_long_fragment",
        FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL }
    },
    { &hf_dtls_fragment_error,
      { "Message defragmentation error", "dtls.fragment.error",
        FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL }
    },
    { &hf_dtls_fragment_count,
      { "Message fragment count", "dtls.fragment.error",
        FT_UINT32, BASE_DEC, NULL, 0x00, NULL, HFILL }
    },
    { &hf_dtls_reassembled_in,
      { "Reassembled in", "dtls.reassembled.in",
        FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL }
    },
    { &hf_dtls_reassembled_length,
      { "Reassembled DTLS length", "dtls.reassembled.length",
        FT_UINT32, BASE_DEC, NULL, 0x00, NULL, HFILL }
    },
  };

  /
  static gint *ett[] = {
    &ett_dtls,
    &ett_dtls_record,
    &ett_dtls_alert,
    &ett_dtls_handshake,
    &ett_dtls_cipher_suites,
    &ett_dtls_comp_methods,
    &ett_dtls_extension,
    &ett_dtls_certs,
    &ett_dtls_cert_types,
    &ett_dtls_dnames,
    &ett_dtls_fragment,
    &ett_dtls_fragments,
  };

  /
  proto_dtls = proto_register_protocol("Datagram Transport Layer Security",
                                       "DTLS", "dtls");

  /
  proto_register_field_array(proto_dtls, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));

#ifdef HAVE_LIBGNUTLS
  {
    module_t *dtls_module = prefs_register_protocol(proto_dtls, dtls_parse);
    prefs_register_string_preference(dtls_module, "keys_list", "RSA keys list",
                                     "semicolon separated list of private RSA keys used for DTLS decryption; "
                                     "each list entry must be in the form of <ip>,<port>,<protocol>,<key_file_name>"
                                     "<key_file_name>   is the local file name of the RSA private key used by the specified server\n",
                                     (const gchar **)&dtls_keys_list);
    prefs_register_string_preference(dtls_module, "debug_file", "DTLS debug file",
                                     "redirect dtls debug to file name; leave empty to disable debug, "
                                     "use \"" SSL_DEBUG_USE_STDERR "\" to redirect output to stderr\n",
                                     (const gchar **)&dtls_debug_file_name);
  }
#endif

  register_dissector("dtls", dissect_dtls, proto_dtls);
  dtls_handle = find_dissector("dtls");

  dtls_associations = g_tree_new(ssl_association_cmp);

  register_init_routine(dtls_init);
  ssl_lib_init();
  dtls_tap = register_tap("dtls");
  ssl_debug_printf("proto_register_dtls: registered tap %s:%d\n",
                   "dtls", dtls_tap);
}
