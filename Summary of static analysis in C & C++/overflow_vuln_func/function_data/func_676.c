void proto_register_snmp(void) {
  /
  static hf_register_info hf[] = {
		{ &hf_snmp_v3_flags_auth,
		{ "Authenticated", "snmp.v3.flags.auth", FT_BOOLEAN, 8,
		    TFS(&tfs_set_notset), TH_AUTH, NULL, HFILL }},
		{ &hf_snmp_v3_flags_crypt,
		{ "Encrypted", "snmp.v3.flags.crypt", FT_BOOLEAN, 8,
		    TFS(&tfs_set_notset), TH_CRYPT, NULL, HFILL }},
		{ &hf_snmp_v3_flags_report,
		{ "Reportable", "snmp.v3.flags.report", FT_BOOLEAN, 8,
		    TFS(&tfs_set_notset), TH_REPORT, NULL, HFILL }},
		{ &hf_snmp_engineid_conform, {
		    "Engine ID Conformance", "snmp.engineid.conform", FT_BOOLEAN, 8,
		    TFS(&tfs_snmp_engineid_conform), F_SNMP_ENGINEID_CONFORM, "Engine ID RFC3411 Conformance", HFILL }},
		{ &hf_snmp_engineid_enterprise, {
		    "Engine Enterprise ID", "snmp.engineid.enterprise", FT_UINT32, BASE_DEC|BASE_EXT_STRING,
		    &sminmpec_values_ext, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_format, {
		    "Engine ID Format", "snmp.engineid.format", FT_UINT8, BASE_DEC,
		    VALS(snmp_engineid_format_vals), 0, NULL, HFILL }},
		{ &hf_snmp_engineid_ipv4, {
		    "Engine ID Data: IPv4 address", "snmp.engineid.ipv4", FT_IPv4, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_ipv6, {
		    "Engine ID Data: IPv6 address", "snmp.engineid.ipv6", FT_IPv6, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_cisco_type, {
		    "Engine ID Data: Cisco type", "snmp.engineid.cisco.type", FT_UINT8, BASE_NONE,
		    VALS(snmp_engineid_cisco_type_vals), 0, NULL, HFILL }},
		{ &hf_snmp_engineid_mac, {
		    "Engine ID Data: MAC address", "snmp.engineid.mac", FT_ETHER, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_text, {
		    "Engine ID Data: Text", "snmp.engineid.text", FT_STRING, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_time, {
		    "Engine ID Data: Creation Time", "snmp.engineid.time", FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_engineid_data, {
		    "Engine ID Data", "snmp.engineid.data", FT_BYTES, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_msgAuthentication, {
		    "Authentication", "snmp.v3.auth", FT_BOOLEAN, BASE_NONE,
		    TFS(&auth_flags), 0, NULL, HFILL }},
		{ &hf_snmp_decryptedPDU, {
	  	    "Decrypted ScopedPDU", "snmp.decrypted_pdu", FT_BYTES, BASE_NONE,
		    NULL, 0, "Decrypted PDU", HFILL }},
		{ &hf_snmp_noSuchObject, {
		    "noSuchObject", "snmp.noSuchObject", FT_NONE, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_noSuchInstance, {
		    "noSuchInstance", "snmp.noSuchInstance", FT_NONE, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_endOfMibView, {
		    "endOfMibView", "snmp.endOfMibView", FT_NONE, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_unSpecified, {
		    "unSpecified", "snmp.unSpecified", FT_NONE, BASE_NONE,
		    NULL, 0, NULL, HFILL }},

		{ &hf_snmp_integer32_value, {
		    "Value (Integer32)", "snmp.value.int", FT_INT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_octetstring_value, {
		    "Value (OctetString)", "snmp.value.octets", FT_BYTES, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_oid_value, {
		    "Value (OID)", "snmp.value.oid", FT_OID, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_null_value, {
		    "Value (Null)", "snmp.value.null", FT_NONE, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_ipv4_value, {
		    "Value (IpAddress)", "snmp.value.ipv4", FT_IPv4, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_ipv6_value, {
		    "Value (IpAddress)", "snmp.value.ipv6", FT_IPv6, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_anyaddress_value, {
		    "Value (IpAddress)", "snmp.value.addr", FT_BYTES, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_unsigned32_value, {
		    "Value (Unsigned32)", "snmp.value.u32", FT_INT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_gauge32_value, {
		    "Value (Gauge32)", "snmp.value.g32", FT_INT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_unknown_value, {
		    "Value (Unknown)", "snmp.value.unk", FT_BYTES, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_counter_value, {
		    "Value (Counter32)", "snmp.value.counter", FT_UINT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_big_counter_value, {
		    "Value (Counter64)", "snmp.value.counter", FT_UINT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_nsap_value, {
		    "Value (NSAP)", "snmp.value.nsap", FT_UINT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_timeticks_value, {
		    "Value (Timeticks)", "snmp.value.timeticks", FT_UINT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_opaque_value, {
		    "Value (Opaque)", "snmp.value.opaque", FT_BYTES, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_objectname, {
		    "Object Name", "snmp.name", FT_OID, BASE_NONE,
		    NULL, 0, NULL, HFILL }},
		{ &hf_snmp_scalar_instance_index, {
		    "Scalar Instance Index", "snmp.name.index", FT_UINT64, BASE_DEC,
		    NULL, 0, NULL, HFILL }},



/
#line 1 "packet-snmp-hfarr.c"
    { &hf_snmp_SMUX_PDUs_PDU,
      { "SMUX-PDUs", "snmp.SMUX_PDUs",
        FT_UINT32, BASE_DEC, VALS(snmp_SMUX_PDUs_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_version,
      { "version", "snmp.version",
        FT_INT32, BASE_DEC, VALS(snmp_Version_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_community,
      { "community", "snmp.community",
        FT_STRING, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_data,
      { "data", "snmp.data",
        FT_UINT32, BASE_DEC, VALS(snmp_PDUs_vals), 0,
        "PDUs", HFILL }},
    { &hf_snmp_parameters,
      { "parameters", "snmp.parameters",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING", HFILL }},
    { &hf_snmp_datav2u,
      { "datav2u", "snmp.datav2u",
        FT_UINT32, BASE_DEC, VALS(snmp_T_datav2u_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_v2u_plaintext,
      { "plaintext", "snmp.plaintext",
        FT_UINT32, BASE_DEC, VALS(snmp_PDUs_vals), 0,
        "PDUs", HFILL }},
    { &hf_snmp_encrypted,
      { "encrypted", "snmp.encrypted",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING", HFILL }},
    { &hf_snmp_msgAuthoritativeEngineID,
      { "msgAuthoritativeEngineID", "snmp.msgAuthoritativeEngineID",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgAuthoritativeEngineBoots,
      { "msgAuthoritativeEngineBoots", "snmp.msgAuthoritativeEngineBoots",
        FT_UINT32, BASE_DEC, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgAuthoritativeEngineTime,
      { "msgAuthoritativeEngineTime", "snmp.msgAuthoritativeEngineTime",
        FT_UINT32, BASE_DEC, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgUserName,
      { "msgUserName", "snmp.msgUserName",
        FT_STRING, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgAuthenticationParameters,
      { "msgAuthenticationParameters", "snmp.msgAuthenticationParameters",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgPrivacyParameters,
      { "msgPrivacyParameters", "snmp.msgPrivacyParameters",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgVersion,
      { "msgVersion", "snmp.msgVersion",
        FT_INT32, BASE_DEC, VALS(snmp_Version_vals), 0,
        "Version", HFILL }},
    { &hf_snmp_msgGlobalData,
      { "msgGlobalData", "snmp.msgGlobalData",
        FT_NONE, BASE_NONE, NULL, 0,
        "HeaderData", HFILL }},
    { &hf_snmp_msgSecurityParameters,
      { "msgSecurityParameters", "snmp.msgSecurityParameters",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgData,
      { "msgData", "snmp.msgData",
        FT_UINT32, BASE_DEC, VALS(snmp_ScopedPduData_vals), 0,
        "ScopedPduData", HFILL }},
    { &hf_snmp_msgID,
      { "msgID", "snmp.msgID",
        FT_UINT32, BASE_DEC, NULL, 0,
        "INTEGER_0_2147483647", HFILL }},
    { &hf_snmp_msgMaxSize,
      { "msgMaxSize", "snmp.msgMaxSize",
        FT_UINT32, BASE_DEC, NULL, 0,
        "INTEGER_484_2147483647", HFILL }},
    { &hf_snmp_msgFlags,
      { "msgFlags", "snmp.msgFlags",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_msgSecurityModel,
      { "msgSecurityModel", "snmp.msgSecurityModel",
        FT_UINT32, BASE_DEC, VALS(sec_models), 0,
        NULL, HFILL }},
    { &hf_snmp_plaintext,
      { "plaintext", "snmp.plaintext",
        FT_NONE, BASE_NONE, NULL, 0,
        "ScopedPDU", HFILL }},
    { &hf_snmp_encryptedPDU,
      { "encryptedPDU", "snmp.encryptedPDU",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_contextEngineID,
      { "contextEngineID", "snmp.contextEngineID",
        FT_BYTES, BASE_NONE, NULL, 0,
        "SnmpEngineID", HFILL }},
    { &hf_snmp_contextName,
      { "contextName", "snmp.contextName",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING", HFILL }},
    { &hf_snmp_get_request,
      { "get-request", "snmp.get_request",
        FT_NONE, BASE_NONE, NULL, 0,
        "GetRequest_PDU", HFILL }},
    { &hf_snmp_get_next_request,
      { "get-next-request", "snmp.get_next_request",
        FT_NONE, BASE_NONE, NULL, 0,
        "GetNextRequest_PDU", HFILL }},
    { &hf_snmp_get_response,
      { "get-response", "snmp.get_response",
        FT_NONE, BASE_NONE, NULL, 0,
        "GetResponse_PDU", HFILL }},
    { &hf_snmp_set_request,
      { "set-request", "snmp.set_request",
        FT_NONE, BASE_NONE, NULL, 0,
        "SetRequest_PDU", HFILL }},
    { &hf_snmp_trap,
      { "trap", "snmp.trap",
        FT_NONE, BASE_NONE, NULL, 0,
        "Trap_PDU", HFILL }},
    { &hf_snmp_getBulkRequest,
      { "getBulkRequest", "snmp.getBulkRequest",
        FT_NONE, BASE_NONE, NULL, 0,
        "GetBulkRequest_PDU", HFILL }},
    { &hf_snmp_informRequest,
      { "informRequest", "snmp.informRequest",
        FT_NONE, BASE_NONE, NULL, 0,
        "InformRequest_PDU", HFILL }},
    { &hf_snmp_sNMPv2_Trap,
      { "sNMPv2-Trap", "snmp.sNMPv2_Trap",
        FT_NONE, BASE_NONE, NULL, 0,
        "SNMPv2_Trap_PDU", HFILL }},
    { &hf_snmp_report,
      { "report", "snmp.report",
        FT_NONE, BASE_NONE, NULL, 0,
        "Report_PDU", HFILL }},
    { &hf_snmp_request_id,
      { "request-id", "snmp.request_id",
        FT_INT32, BASE_DEC, NULL, 0,
        "INTEGER", HFILL }},
    { &hf_snmp_error_status,
      { "error-status", "snmp.error_status",
        FT_INT32, BASE_DEC, VALS(snmp_T_error_status_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_error_index,
      { "error-index", "snmp.error_index",
        FT_INT32, BASE_DEC, NULL, 0,
        "INTEGER", HFILL }},
    { &hf_snmp_variable_bindings,
      { "variable-bindings", "snmp.variable_bindings",
        FT_UINT32, BASE_DEC, NULL, 0,
        "VarBindList", HFILL }},
    { &hf_snmp_bulkPDU_request_id,
      { "request-id", "snmp.request_id",
        FT_INT32, BASE_DEC, NULL, 0,
        "Integer32", HFILL }},
    { &hf_snmp_non_repeaters,
      { "non-repeaters", "snmp.non_repeaters",
        FT_UINT32, BASE_DEC, NULL, 0,
        "INTEGER_0_2147483647", HFILL }},
    { &hf_snmp_max_repetitions,
      { "max-repetitions", "snmp.max_repetitions",
        FT_UINT32, BASE_DEC, NULL, 0,
        "INTEGER_0_2147483647", HFILL }},
    { &hf_snmp_enterprise,
      { "enterprise", "snmp.enterprise",
        FT_OID, BASE_NONE, NULL, 0,
        "EnterpriseOID", HFILL }},
    { &hf_snmp_agent_addr,
      { "agent-addr", "snmp.agent_addr",
        FT_IPv4, BASE_NONE, NULL, 0,
        "NetworkAddress", HFILL }},
    { &hf_snmp_generic_trap,
      { "generic-trap", "snmp.generic_trap",
        FT_INT32, BASE_DEC, VALS(snmp_GenericTrap_vals), 0,
        "GenericTrap", HFILL }},
    { &hf_snmp_specific_trap,
      { "specific-trap", "snmp.specific_trap",
        FT_INT32, BASE_DEC, NULL, 0,
        "SpecificTrap", HFILL }},
    { &hf_snmp_time_stamp,
      { "time-stamp", "snmp.time_stamp",
        FT_UINT32, BASE_DEC, NULL, 0,
        "TimeTicks", HFILL }},
    { &hf_snmp_name,
      { "name", "snmp.name",
        FT_OID, BASE_NONE, NULL, 0,
        "ObjectName", HFILL }},
    { &hf_snmp_valueType,
      { "valueType", "snmp.valueType",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_VarBindList_item,
      { "VarBind", "snmp.VarBind",
        FT_NONE, BASE_NONE, NULL, 0,
        NULL, HFILL }},
    { &hf_snmp_open,
      { "open", "snmp.open",
        FT_UINT32, BASE_DEC, VALS(snmp_OpenPDU_vals), 0,
        "OpenPDU", HFILL }},
    { &hf_snmp_close,
      { "close", "snmp.close",
        FT_INT32, BASE_DEC, VALS(snmp_ClosePDU_U_vals), 0,
        "ClosePDU", HFILL }},
    { &hf_snmp_registerRequest,
      { "registerRequest", "snmp.registerRequest",
        FT_NONE, BASE_NONE, NULL, 0,
        "RReqPDU", HFILL }},
    { &hf_snmp_registerResponse,
      { "registerResponse", "snmp.registerResponse",
        FT_UINT32, BASE_DEC, VALS(snmp_RegisterResponse_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_commitOrRollback,
      { "commitOrRollback", "snmp.commitOrRollback",
        FT_INT32, BASE_DEC, VALS(snmp_SOutPDU_U_vals), 0,
        "SOutPDU", HFILL }},
    { &hf_snmp_rRspPDU,
      { "rRspPDU", "snmp.rRspPDU",
        FT_INT32, BASE_DEC, VALS(snmp_RRspPDU_U_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_pDUs,
      { "pDUs", "snmp.pDUs",
        FT_UINT32, BASE_DEC, VALS(snmp_PDUs_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_smux_simple,
      { "smux-simple", "snmp.smux_simple",
        FT_NONE, BASE_NONE, NULL, 0,
        "SimpleOpen", HFILL }},
    { &hf_snmp_smux_version,
      { "smux-version", "snmp.smux_version",
        FT_INT32, BASE_DEC, VALS(snmp_T_smux_version_vals), 0,
        NULL, HFILL }},
    { &hf_snmp_identity,
      { "identity", "snmp.identity",
        FT_OID, BASE_NONE, NULL, 0,
        "OBJECT_IDENTIFIER", HFILL }},
    { &hf_snmp_description,
      { "description", "snmp.description",
        FT_BYTES, BASE_NONE, NULL, 0,
        "DisplayString", HFILL }},
    { &hf_snmp_password,
      { "password", "snmp.password",
        FT_BYTES, BASE_NONE, NULL, 0,
        "OCTET_STRING", HFILL }},
    { &hf_snmp_subtree,
      { "subtree", "snmp.subtree",
        FT_OID, BASE_NONE, NULL, 0,
        "ObjectName", HFILL }},
    { &hf_snmp_priority,
      { "priority", "snmp.priority",
        FT_INT32, BASE_DEC, NULL, 0,
        "INTEGER_M1_2147483647", HFILL }},
    { &hf_snmp_operation,
      { "operation", "snmp.operation",
        FT_INT32, BASE_DEC, VALS(snmp_T_operation_vals), 0,
        NULL, HFILL }},

/
#line 2133 "packet-snmp-template.c"
  };

  /
  static gint *ett[] = {
	  &ett_snmp,
	  &ett_engineid,
	  &ett_msgFlags,
	  &ett_encryptedPDU,
	  &ett_decrypted,
	  &ett_authParameters,
	  &ett_internet,
	  &ett_varbind,
	  &ett_name,
	  &ett_value,
	  &ett_decoding_error,

/
#line 1 "packet-snmp-ettarr.c"
    &ett_snmp_Message,
    &ett_snmp_Messagev2u,
    &ett_snmp_T_datav2u,
    &ett_snmp_UsmSecurityParameters,
    &ett_snmp_SNMPv3Message,
    &ett_snmp_HeaderData,
    &ett_snmp_ScopedPduData,
    &ett_snmp_ScopedPDU,
    &ett_snmp_PDUs,
    &ett_snmp_PDU,
    &ett_snmp_BulkPDU,
    &ett_snmp_Trap_PDU_U,
    &ett_snmp_VarBind,
    &ett_snmp_VarBindList,
    &ett_snmp_SMUX_PDUs,
    &ett_snmp_RegisterResponse,
    &ett_snmp_OpenPDU,
    &ett_snmp_SimpleOpen_U,
    &ett_snmp_RReqPDU_U,

/
#line 2149 "packet-snmp-template.c"
  };
  module_t *snmp_module;

  static uat_field_t users_fields[] = {
	  UAT_FLD_BUFFER(snmp_users,engine_id,"Engine ID","Engine-id for this entry (empty = any)"),
	  UAT_FLD_LSTRING(snmp_users,userName,"Username","The username"),
	  UAT_FLD_VS(snmp_users,auth_model,"Authentication model",auth_types,"Algorithm to be used for authentication."),
	  UAT_FLD_LSTRING(snmp_users,authPassword,"Password","The password used for authenticating packets for this entry"),
	  UAT_FLD_VS(snmp_users,priv_proto,"Privacy protocol",priv_types,"Algorithm to be used for privacy."),
	  UAT_FLD_LSTRING(snmp_users,privPassword,"Privacy password","The password used for encrypting packets for this entry"),
	  UAT_END_FIELDS
  };

  uat_t *assocs_uat = uat_new("SNMP Users",
			      sizeof(snmp_ue_assoc_t),
			      "snmp_users",
			      TRUE,
			      (void*)&ueas,
			      &num_ueas,
			      UAT_CAT_CRYPTO,
			      "ChSNMPUsersSection",
			      snmp_users_copy_cb,
			      snmp_users_update_cb,
			      snmp_users_free_cb,
			      renew_ue_cache,
			      users_fields);

  static uat_field_t specific_traps_flds[] = {
    UAT_FLD_CSTRING(specific_traps,enterprise,"Enterprise OID","Enterprise Object Identifier"),
    UAT_FLD_DEC(specific_traps,trap,"Trap Id","The specific-trap value"),
    UAT_FLD_CSTRING(specific_traps,desc,"Description","Trap type description"),
    UAT_END_FIELDS
  };

  uat_t* specific_traps_uat = uat_new("SNMP Enterprise Specific Trap Types",
                                      sizeof(snmp_st_assoc_t),
                                      "snmp_specific_traps",
                                      TRUE,
                                      (void*) &specific_traps,
                                      &num_specific_traps,
                                      UAT_CAT_GENERAL,
                                      "ChSNMPEnterpriseSpecificTrapTypes",
                                      snmp_specific_trap_copy_cb,
                                      NULL,
                                      snmp_specific_trap_free_cb,
									  NULL,
                                      specific_traps_flds);

  /
  proto_snmp = proto_register_protocol(PNAME, PSNAME, PFNAME);
  new_register_dissector("snmp", dissect_snmp, proto_snmp);

  /
  proto_register_field_array(proto_snmp, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));


  /
  snmp_module = prefs_register_protocol(proto_snmp, process_prefs);
  prefs_register_bool_preference(snmp_module, "display_oid",
			"Show SNMP OID in info column",
			"Whether the SNMP OID should be shown in the info column",
			&display_oid);

  prefs_register_obsolete_preference(snmp_module, "mib_modules");
  prefs_register_obsolete_preference(snmp_module, "users_file");

  prefs_register_bool_preference(snmp_module, "desegment",
			"Reassemble SNMP-over-TCP messages\nspanning multiple TCP segments",
			"Whether the SNMP dissector should reassemble messages spanning multiple TCP segments."
			" To use this option, you must also enable \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings.",
			&snmp_desegment);

  prefs_register_bool_preference(snmp_module, "var_in_tree",
			"Display dissected variables inside SNMP tree",
			"ON - display dissected variables inside SNMP tree, OFF - display dissected variables in root tree after SNMP",
			&snmp_var_in_tree);

  prefs_register_uat_preference(snmp_module, "users_table",
				"Users Table",
				"Table of engine-user associations used for authentication and decryption",
				assocs_uat);

  prefs_register_uat_preference(snmp_module, "specific_traps_table",
				"Enterprise Specific Trap Types",
				"Table of enterprise specific-trap type descriptions",
				specific_traps_uat);

#ifdef HAVE_LIBSMI
  prefs_register_static_text_preference(snmp_module, "info_mibs",
				        "MIB settings can be changed in the Name Resolution preferences",
				        "MIB settings can be changed in the Name Resolution preferences");
#endif

  value_sub_dissectors_table = register_dissector_table("snmp.variable_oid","SNMP Variable OID", FT_STRING, BASE_NONE);

  register_init_routine(renew_ue_cache);

  register_ber_syntax_dissector("SNMP", proto_snmp, dissect_snmp_tcp);
}
