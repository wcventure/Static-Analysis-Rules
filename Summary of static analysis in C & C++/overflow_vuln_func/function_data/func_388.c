void
proto_register_eap(void)
{
  static hf_register_info hf[] = {
     { &hf_eap_code, {
      "Code", "eap.code",
      FT_UINT8, BASE_DEC, VALS(eap_code_vals), 0x0,
      NULL, HFILL }},

    { &hf_eap_identifier, {
      "Id", "eap.id",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_len, {
      "Length", "eap.len",
      FT_UINT16, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_type, {
      "Type", "eap.type",
      FT_UINT8, BASE_DEC|BASE_EXT_STRING, &eap_type_vals_ext, 0x0,
      NULL, HFILL }},

    { &hf_eap_type_nak, {
      "Desired Auth Type", "eap.desired_type",
      FT_UINT8, BASE_DEC|BASE_EXT_STRING, &eap_type_vals_ext, 0x0,
      NULL, HFILL }},

    { &hf_eap_identity, {
      "Identity", "eap.identity",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_identity_prefix, {
      "Identity", "eap.identity.prefix",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_identity_actual_len, {
      "Identity Actual Length", "eap.identity.actual_len",
      FT_UINT16, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_notification, {
      "Notification", "eap.notification",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_md5_value_size, {
      "EAP-MD5 Value-Size", "eap.md5.value_size",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_md5_value, {
      "EAP-MD5 Value", "eap.md5.value",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_md5_extra_data, {
      "EAP-MD5 Extra Data", "eap.md5.extra_data",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_tls_flags, {
      "EAP-TLS Flags", "eap.tls.flags",
      FT_UINT8, BASE_HEX, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_tls_flag_l, {
      "Length Included", "eap.tls.flags.len_included",
      FT_BOOLEAN, 8, NULL, EAP_TLS_FLAG_L,
      NULL, HFILL }},

    { &hf_eap_tls_flag_m, {
      "More Fragments", "eap.tls.flags.more_fragments",
      FT_BOOLEAN, 8, NULL, EAP_TLS_FLAG_M,
      NULL, HFILL }},

    { &hf_eap_tls_flag_s, {
      "Start", "eap.tls.flags.start",
      FT_BOOLEAN, 8, NULL, EAP_TLS_FLAG_S,
      NULL, HFILL }},

    { &hf_eap_tls_flags_version, {
      "Version", "eap.tls.flags.version",
      FT_UINT8, BASE_DEC, NULL, EAP_TLS_FLAGS_VERSION,
      NULL, HFILL }},

    { &hf_eap_tls_len, {
      "EAP-TLS Length", "eap.tls.len",
      FT_UINT32, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_tls_fragment, {
      "EAP-TLS Fragment", "eap.tls.fragment",
      FT_FRAMENUM, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_tls_fragments, {
      "EAP-TLS Fragments", "eap.tls.fragments",
      FT_NONE, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_tls_fragment_overlap, {
      "Fragment Overlap", "eap.tls.fragment.overlap",
      FT_BOOLEAN, BASE_NONE, NULL, 0x0,
      "Fragment overlaps with other fragments", HFILL }},

    { &hf_eap_tls_fragment_overlap_conflict, {
      "Conflicting Data In Fragment Overlap", "eap.tls.fragment.overlap_conflict",
      FT_BOOLEAN, BASE_NONE, NULL, 0x0,
      "Overlapping fragments contained conflicting data", HFILL }},

    { &hf_eap_tls_fragment_multiple_tails, {
      "Multiple Tail Fragments Found", "eap.tls.fragment.multiple_tails",
      FT_BOOLEAN, BASE_NONE, NULL, 0x0,
      "Several tails were found when defragmenting the packet", HFILL }},

    { &hf_eap_tls_fragment_too_long_fragment,{
      "Fragment Too Long", "eap.tls.fragment.fragment.too_long",
      FT_BOOLEAN, BASE_NONE, NULL, 0x0,
      "Fragment contained data past end of packet", HFILL }},

    { &hf_eap_tls_fragment_error, {
      "Defragmentation Error", "eap.tls.fragment.error",
      FT_FRAMENUM, BASE_NONE, NULL, 0x0,
      "Defragmentation error due to illegal fragments", HFILL }},

    { &hf_eap_tls_fragment_count, {
      "Fragment Count", "eap.tls.fragment.count",
      FT_UINT32, BASE_DEC, NULL, 0x0,
      "Total length of the reassembled payload", HFILL }},

    { &hf_eap_tls_reassembled_length, {
      "Reassembled EAP-TLS Length", "eap.tls.reassembled.len",
      FT_UINT32, BASE_DEC, NULL, 0x0,
      "Total length of the reassembled payload", HFILL }},

    { &hf_eap_sim_subtype, {
      "EAP-SIM Subtype", "eap.sim.subtype",
      FT_UINT8, BASE_DEC, VALS(eap_sim_subtype_vals), 0x0,
      NULL, HFILL }},

    { &hf_eap_sim_reserved, {
      "EAP-SIM Reserved", "eap.sim.reserved",
      FT_UINT16, BASE_HEX, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_sim_subtype_attribute, {
      "EAP-SIM Attribute", "eap.sim.subtype.attribute",
      FT_NONE, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_sim_subtype_type, {
      "EAP-SIM Type", "eap.sim.subtype.type",
      FT_UINT8, BASE_DEC|BASE_EXT_STRING, &eap_sim_aka_attribute_vals_ext, 0x0,
      NULL, HFILL }},

    { &hf_eap_sim_subtype_length, {
      "EAP-SIM Length", "eap.sim.subtype.len",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_sim_subtype_value, {
      "EAP-SIM Value", "eap.sim.subtype.value",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_subtype, {
      "EAP-AKA Subtype", "eap.aka.subtype",
      FT_UINT8, BASE_DEC, VALS(eap_aka_subtype_vals), 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_reserved, {
      "EAP-AKA Reserved", "eap.aka.reserved",
      FT_UINT16, BASE_HEX, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_subtype_attribute, {
      "EAP-AKA Attribute", "eap.aka.subtype.attribute",
      FT_NONE, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_subtype_type, {
      "EAP-AKA Type", "eap.aka.subtype.type",
      FT_UINT8, BASE_DEC|BASE_EXT_STRING, &eap_sim_aka_attribute_vals_ext, 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_subtype_length, {
      "EAP-AKA Length", "eap.aka.subtype.len",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_aka_subtype_value, {
      "EAP-AKA Value", "eap.aka.subtype.value",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_version, {
      "EAP-LEAP Version", "eap.leap.version",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_reserved, {
      "EAP-LEAP Reserved", "eap.leap.reserved",
      FT_UINT8, BASE_HEX, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_count, {
      "EAP-LEAP Count", "eap.leap.count",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_peer_challenge, {
      "EAP-LEAP Peer-Challenge", "eap.leap.peer_challenge",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_peer_response, {
      "EAP-LEAP Peer-Response", "eap.leap.peer_response",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_ap_challenge, {
      "EAP-LEAP AP-Challenge", "eap.leap.ap_challenge",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_ap_response, {
      "EAP-LEAP AP-Response", "eap.leap.ap_response",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_data, {
      "EAP-LEAP Data", "eap.leap.data",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_leap_name, {
      "EAP-LEAP Name", "eap.leap.name",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_opcode, {
      "EAP-MS-CHAP-v2 OpCode", "eap.ms_chap_v2.opcode",
      FT_UINT8, BASE_DEC, VALS(eap_ms_chap_v2_opcode_vals), 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_id, {
      "EAP-MS-CHAP-v2 Id", "eap.ms_chap_v2.id",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_length, {
      "EAP-MS-CHAP-v2 Length", "eap.ms_chap_v2.length",
      FT_UINT16, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_value_size, {
      "EAP-MS-CHAP-v2 Value-Size", "eap.ms_chap_v2.value_size",
      FT_UINT8, BASE_DEC, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_challenge, {
      "EAP-MS-CHAP-v2 Challenge", "eap.ms_chap_v2.challenge",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_name, {
      "EAP-MS-CHAP-v2 Name", "eap.ms_chap_v2.name",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_peer_challenge, {
      "EAP-MS-CHAP-v2 Peer-Challenge", "eap.ms_chap_v2.peer_challenge",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_reserved, {
      "EAP-MS-CHAP-v2 Reserved", "eap.ms_chap_v2.reserved",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_nt_response, {
      "EAP-MS-CHAP-v2 NT-Response", "eap.ms_chap_v2.nt_response",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_flags, {
      "EAP-MS-CHAP-v2 Flags", "eap.ms_chap_v2.flags",
      FT_UINT8, BASE_HEX, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_response, {
      "EAP-MS-CHAP-v2 Response (Unknown Length)", "eap.ms_chap_v2.response",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_message, {
      "EAP-MS-CHAP-v2 Message", "eap.ms_chap_v2.message",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_failure_request, {
      "EAP-MS-CHAP-v2 Failure-Request", "eap.ms_chap_v2.failure_request",
      FT_STRING, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_ms_chap_v2_data, {
      "EAP-MS-CHAP-v2 Data", "eap.ms_chap_v2.data",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    { &hf_eap_data, {
      "EAP Data", "eap.data",
      FT_BYTES, BASE_NONE, NULL, 0x0,
      NULL, HFILL }},

    /
    { &hf_eap_ext_vendor_id, {
      "EAP-EXT Vendor Id", "eap.ext.vendor_id",
      FT_UINT16, BASE_HEX, VALS(eap_ext_vendor_id_vals), 0x0,
      NULL, HFILL }},

    { &hf_eap_ext_vendor_type, {
      "EAP-EXT Vendor Type", "eap.ext.vendor_type",
      FT_UINT8, BASE_HEX, VALS(eap_ext_vendor_type_vals), 0x0,
      NULL, HFILL }}
  };
  static gint *ett[] = {
    &ett_eap,
    &ett_eap_tls_fragment,
    &ett_eap_tls_fragments,
    &ett_eap_sim_attr,
    &ett_eap_aka_attr,
    &ett_eap_exp_attr,
    &ett_eap_tls_flags,
    &ett_identity
  };
  static ei_register_info ei[] = {
     { &ei_eap_ms_chap_v2_length, { "eap.ms_chap_v2.length.invalid", PI_PROTOCOL, PI_WARN, "Invalid Length", EXPFILL }},
     { &ei_eap_mitm_attacks, { "eap.mitm_attacks", PI_SECURITY, PI_WARN, "Vulnerable to MITM attacks. If possible, change EAP type.", EXPFILL }},
     { &ei_eap_md5_value_size_overflow, { "eap.md5.value_size.overflow", PI_PROTOCOL, PI_WARN, "Overflow", EXPFILL }},
     { &ei_eap_dictionary_attacks, { "eap.dictionary_attacks", PI_SECURITY, PI_WARN,
                               "Vulnerable to dictionary attacks. If possible, change EAP type."
                               " See http://www.cisco.com/warp/public/cc/pd/witc/ao350ap/prodlit/2331_pp.pdf", EXPFILL }},
  };

  expert_module_t* expert_eap;

  proto_eap = proto_register_protocol("Extensible Authentication Protocol",
                                      "EAP", "eap");
  proto_register_field_array(proto_eap, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  expert_eap = expert_register_protocol(proto_eap);
  expert_register_field_array(expert_eap, ei, array_length(ei));

  eap_handle = register_dissector("eap", dissect_eap, proto_eap);
  register_init_routine(eap_tls_defragment_init);
  register_cleanup_routine(eap_tls_defragment_cleanup);
}
