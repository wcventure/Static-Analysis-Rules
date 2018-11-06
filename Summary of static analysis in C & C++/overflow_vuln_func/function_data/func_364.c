void
proto_register_mgcp(void)
{
  static hf_register_info hf[] = {
    { &hf_mgcp_req,
      { "Request", "mgcp.req", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
	"True if MGCP request", HFILL }},
    { &hf_mgcp_rsp,
      { "Response", "mgcp.rsp", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
	"TRUE if MGCP response", HFILL }},
    { &hf_mgcp_req_frame, 
      {	"Request Frame", "mgcp.reqframe", FT_FRAMENUM, BASE_NONE, NULL, 0,
        "Request Frame", HFILL }},
    { &hf_mgcp_rsp_frame, 
      {	"Response Frame", "mgcp.rspframe", FT_FRAMENUM, BASE_NONE, NULL, 0,
        "Response Frame", HFILL }},
    { &hf_mgcp_time,
      {	"Time from request", "mgcp.time", FT_RELATIVE_TIME, BASE_NONE, NULL, 0,
        "Timedelta between Request and Response", HFILL }},
    { &hf_mgcp_req_verb,
      { "Verb", "mgcp.req.verb", FT_STRING, BASE_DEC, NULL, 0x0,
	"Name of the verb", HFILL }},
    { &hf_mgcp_req_endpoint,
      { "Endpoint", "mgcp.req.endpoint", FT_STRING, BASE_DEC, NULL, 0x0,
	"Endpoint referenced by the message", HFILL }},
    { &hf_mgcp_transid,
      { "Transaction ID", "mgcp.transid", FT_STRING, BASE_DEC, NULL, 0x0,
	"Transaction ID of this message", HFILL }},
    { &hf_mgcp_version,
      { "Version", "mgcp.version", FT_STRING, BASE_DEC, NULL, 0x0,
	"MGCP Version", HFILL }},
    { &hf_mgcp_rsp_rspcode,
      { "Response Code", "mgcp.rsp.rspcode", FT_STRING, BASE_DEC, NULL, 0x0,
	"Response Code", HFILL }},
    { &hf_mgcp_rsp_rspstring,
      { "Response String", "mgcp.rsp.rspstring", FT_STRING, BASE_DEC, NULL,
	0x0, "Response String", HFILL }},
    { &hf_mgcp_param_rspack,
      { "ResponseAck (K)", "mgcp.param.rspack", FT_STRING, BASE_DEC, NULL,
	0x0, "Response Ack", HFILL }},
    { &hf_mgcp_param_bearerinfo,
      { "BearerInformation (B)", "mgcp.param.bearerinfo", FT_STRING, BASE_DEC,
	NULL, 0x0, "Bearer Information", HFILL }},
    { &hf_mgcp_param_callid,
      { "CallId (C)", "mgcp.param.callid", FT_STRING, BASE_DEC, NULL, 0x0,
	"Call Id", HFILL }},
    { &hf_mgcp_param_connectionid,
      {"ConnectionIdentifier (I)", "mgcp.param.connectionid", FT_STRING,
       BASE_DEC, NULL, 0x0, "Connection Identifier", HFILL }},
    { &hf_mgcp_param_secondconnectionid,
      { "SecondConnectionID (I2)", "mgcp.param.secondconnectionid", FT_STRING,
       BASE_DEC, NULL, 0x0, "Second Connection Identifier", HFILL }},
    { &hf_mgcp_param_notifiedentity,
      { "NotifiedEntity (N)", "mgcp.param.notifiedentity", FT_STRING, BASE_DEC,
	NULL, 0x0, "Notified Entity", HFILL }},
    { &hf_mgcp_param_requestid,
      { "RequestIdentifier (X)", "mgcp.param.requestid", FT_STRING, BASE_DEC,
	NULL, 0x0, "Request Identifier", HFILL }},
    { &hf_mgcp_param_localconnoptions,
      { "LocalConnectionOptions (L)", "mgcp.param.localconnectionoptions",
	FT_STRING, BASE_DEC, NULL, 0x0, "Local Connection Options", HFILL }},
    { &hf_mgcp_param_connectionmode,
      { "ConnectionMode (M)", "mgcp.param.connectionmode", FT_STRING, BASE_DEC,
	NULL, 0x0, "Connection Mode", HFILL }},
    { &hf_mgcp_param_reqevents,
      { "RequestedEvents (R)", "mgcp.param.reqevents", FT_STRING, BASE_DEC,
	NULL, 0x0, "Requested Events", HFILL }},
    { &hf_mgcp_param_signalreq,
      { "SignalRequests (S)", "mgcp.param.signalreq", FT_STRING, BASE_DEC,
	NULL, 0x0, "Signal Request", HFILL }},
    { &hf_mgcp_param_restartmethod,
      { "RestartMethod (RM)", "mgcp.param.restartmethod", FT_STRING, BASE_DEC,
	NULL, 0x0, "Restart Method", HFILL }},
    { &hf_mgcp_param_restartdelay,
      { "RestartDelay (RD)", "mgcp.param.restartdelay", FT_STRING, BASE_DEC,
	NULL, 0x0, "Restart Delay", HFILL }},
    { &hf_mgcp_param_digitmap,
      { "DigitMap (D)", "mgcp.param.digitmap", FT_STRING, BASE_DEC, NULL, 0x0,
	"Digit Map", HFILL }},
    { &hf_mgcp_param_observedevent,
      { "ObservedEvents (O)", "mgcp.param.observedevents", FT_STRING,
	BASE_DEC, NULL, 0x0, "Observed Events", HFILL }},
    { &hf_mgcp_param_connectionparam,
      { "ConnectionParameters (P)", "mgcp.param.connectionparam", FT_STRING,
	BASE_DEC, NULL, 0x0, "Connection Parameters", HFILL }},
    { &hf_mgcp_param_reasoncode,
      { "ReasonCode (E)", "mgcp.param.reasoncode", FT_STRING, BASE_DEC,
	NULL, 0x0, "Reason Code", HFILL }},
    { &hf_mgcp_param_eventstates,
      { "EventStates (ES)", "mgcp.param.eventstates", FT_STRING, BASE_DEC,
	NULL, 0x0, "Event States", HFILL }},
    { &hf_mgcp_param_specificendpoint,
      { "SpecificEndpointID (Z)", "mgcp.param.specificendpointid", FT_STRING,
	BASE_DEC, NULL, 0x0, "Specific Endpoint ID", HFILL }},
    { &hf_mgcp_param_secondendpointid,
      { "SecondEndpointID (Z2)", "mgcp.param.secondendpointid", FT_STRING,
	BASE_DEC, NULL, 0x0, "Second Endpoing ID", HFILL }},
    { &hf_mgcp_param_reqinfo,
      { "RequestedInfo (F)", "mgcp.param.reqinfo", FT_STRING, BASE_DEC,
	NULL, 0x0,"Requested Info", HFILL }},
    { &hf_mgcp_param_quarantinehandling,
      { "QuarantineHandling (Q)", "mgcp.param.quarantinehandling", FT_STRING,
	BASE_DEC, NULL, 0x0, "Quarantine Handling", HFILL }},
    { &hf_mgcp_param_detectedevents,
      { "DetectedEvents (T)", "mgcp.param.detectedevents", FT_STRING, BASE_DEC,
	NULL, 0x0, "Detected Events", HFILL }},
    { &hf_mgcp_param_capabilities,
      { "Capabilities (A)", "mgcp.param.capabilities", FT_STRING, BASE_DEC,
	NULL, 0x0, "Capabilities", HFILL }},
    { &hf_mgcp_param_extention,
      { "Extention Parameter (X-*)", "mgcp.param.extention", FT_STRING,
	BASE_DEC, NULL, 0x0, "Extension Parameter", HFILL }},
    { &hf_mgcp_param_invalid,
      { "Invalid Parameter", "mgcp.param.invalid", FT_STRING,
	BASE_DEC, NULL, 0x0, "Invalid Parameter", HFILL }},
    { &hf_mgcp_messagecount,
      { "MGCP Message Count", "mgcp.messagecount", FT_UINT32,
	BASE_DEC, NULL, 0x0, "Number of MGCP message in a packet", HFILL }},
    { &hf_mgcp_dup,
      {	"Duplicate Message", "mgcp.dup", FT_UINT32, BASE_DEC,
	NULL, 0, "Duplicate Message", HFILL }},
    { &hf_mgcp_req_dup,
      {	"Duplicate Request", "mgcp.req.dup", FT_UINT32, BASE_DEC,
	NULL, 0, "Duplicate Request", HFILL }},
    { &hf_mgcp_rsp_dup,
      {	"Duplicate Response", "mgcp.rsp.dup", FT_UINT32, BASE_DEC,
	NULL, 0, "Duplicate Response", HFILL }},
    /
  };
  static gint *ett[] = {
    &ett_mgcp,
    &ett_mgcp_param,
  };
  module_t *mgcp_module;

  proto_mgcp = proto_register_protocol("Media Gateway Control Protocol",
				       "MGCP", "mgcp");

  proto_register_field_array(proto_mgcp, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  register_init_routine(&mgcp_init_protocol);

  /

  mgcp_module = prefs_register_protocol(proto_mgcp, proto_reg_handoff_mgcp);

  prefs_register_uint_preference(mgcp_module, "tcp.gateway_port",
				 "MGCP Gateway TCP Port",
				 "Set the UDP port for gateway messages "
				 "(if other than the default of 2427)",
				 10, &global_mgcp_gateway_tcp_port);

  prefs_register_uint_preference(mgcp_module, "udp.gateway_port",
				 "MGCP Gateway UDP Port",
				 "Set the TCP port for gateway messages "
				 "(if other than the default of 2427)",
				 10, &global_mgcp_gateway_udp_port);

  prefs_register_uint_preference(mgcp_module, "tcp.callagent_port",
				 "MGCP Callagent TCP Port",
				 "Set the TCP port for callagent messages "
				 "(if other than the default of 2727)",
				 10, &global_mgcp_callagent_tcp_port);

  prefs_register_uint_preference(mgcp_module, "udp.callagent_port",
				 "MGCP Callagent UDP Port",
				 "Set the UDP port for callagent messages "
				 "(if other than the default of 2727)",
				 10, &global_mgcp_callagent_udp_port);


  prefs_register_bool_preference(mgcp_module, "display_raw_text",
                                 "Display raw text for MGCP message",
                                 "Specifies that the raw text of the "
                                 "MGCP message should be displayed "
				 "instead of (or in addition to) the "
				 "dissection tree",
                                 &global_mgcp_raw_text);

  prefs_register_bool_preference(mgcp_module, "display_dissect_tree",
                                 "Display tree dissection for MGCP message",
                                 "Specifies that the dissection tree of the "
                                 "MGCP message should be displayed "
				 "instead of (or in addition to) the "
				 "raw text",
                                 &global_mgcp_dissect_tree);

  prefs_register_bool_preference(mgcp_module, "display_mgcp_message_count",
                                 "Display the number of MGCP messages",
                                 "Display the number of MGCP messages "
                                 "found in a packet in the protocol column.",
                                 &global_mgcp_message_count);
  
  mgcp_tap = register_tap("mgcp");
}
