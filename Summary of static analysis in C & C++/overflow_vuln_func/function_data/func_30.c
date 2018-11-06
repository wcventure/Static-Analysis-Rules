void proto_register_ua_msg(void)
{
	module_t *ua_msg_module;

	static gint *ett[] =
	{
		&ett_ua_msg,
	};

	/
	rtp_handle = find_dissector("rtp");
	rtcp_handle = find_dissector("rtcp");

	/
	proto_ua_msg = proto_register_protocol("Universal Alcatel Protocol", "UA", "ua");

	register_dissector("ua_sys_to_term", dissect_ua_sys_to_term, proto_ua_msg);
	register_dissector("ua_term_to_sys", dissect_ua_term_to_sys, proto_ua_msg);

	/
	proto_register_subtree_array(ett, array_length(ett));

	/
	ua_msg_module = prefs_register_protocol(proto_ua_msg, proto_reg_handoff_ua_msg);

	prefs_register_bool_preference(ua_msg_module, "setup_conversations",
		"Setup RTP/RTCP conversations on Start RTP",
		"Setup RTP/RTCP conversations when parsing Start RTP messages",
		&setup_conversations_enabled);
}
