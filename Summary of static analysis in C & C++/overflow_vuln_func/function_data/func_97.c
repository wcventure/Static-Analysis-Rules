void proto_register_uaudp(void)
{
	module_t *uaudp_module;
	int i;

	/
	static hf_register_info hf_uaudp[] =
	{
		{ 
			&hf_uaudp_opcode,
			{
				"Opcode",
				 "uaudp.opcode",
				 FT_UINT8,
				 BASE_DEC,
				 VALS(uaudp_opcode_str),
				 0x0,
				 "UA/UDP Opcode",
				 HFILL
			}
		},
		{ 
			&hf_uaudp_version,
			{
				"Version",
				"uaudp.version",
				FT_UINT8,
				BASE_DEC,
				NULL, 0x0,
				"UA/UDP Version",
				HFILL
			}
		},
		{ 
			&hf_uaudp_window_size,
			{
				"Window Size",
				"uaudp.window_size", 
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP Window Size",
				HFILL
			}
		},
		{
			&hf_uaudp_mtu,
			{
				"MTU", 
				"uaudp.mtu",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP MTU",
				HFILL
			}
		},
		{ 
			&hf_uaudp_udp_lost,
			{
				"UDP Lost",
				"uaudp.udp_lost",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP Lost",
				HFILL
			}
		},
		{ 
			&hf_uaudp_udp_lost_reinit,
			{ 
				"UDP lost reinit",
				"uaudp.udp_lost_reinit",
				FT_UINT8,
				BASE_DEC,
				NULL, 0x0,
				"UA/UDP Lost Re-Init",
				HFILL
			}
		},
		{ 
			&hf_uaudp_keepalive,
			{ 
				"Keepalive",
				"uaudp.keepalive",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP Keepalive",
				HFILL
			}
		},
		{ 
			&hf_uaudp_qos_ip_tos,
			{
				"QoS IP TOS",
				"uaudp.qos_ip_tos",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP QoS IP TOS",
				HFILL
			}
		},
		{ 
			&hf_uaudp_qos_8021_vlid,
			{ 
				"QoS 802.1 VLID",
				"uaudp.qos_8021_vlid",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP QoS 802.1 VLID",
				HFILL
			}
		},
		{ 
			&hf_uaudp_qos_8021_pri,
			{ 
				"QoS 802.1 PRI",
				"uaudp.qos_8021_pri",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP QoS 802.1 PRI",
				HFILL
			}
		},
		{ 
			&hf_uaudp_expseq,
			{
				"Sequence Number (expected)",
				"uaudp.expseq",
				FT_UINT16,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP Expected Sequence Number",
				HFILL
			}
		},
		{ 
			&hf_uaudp_sntseq,
			{ 
				"Sequence Number (sent)", 
				"uaudp.sntseq",
				FT_UINT16,
				BASE_DEC,
				NULL,
				0x0,
				"UA/UDP Sent Sequence Number",
				HFILL
			}
		},
	};

	/
	static gint *ett[] =
	{
		&ett_uaudp,
	};

	/
	proto_uaudp = proto_register_protocol("UA/UDP Encapsulation Protocol", 
										  "UAUDP",
										  "uaudp");

	register_dissector("uaudp", dissect_uaudp, proto_uaudp);
	register_dissector("uaudp_dir_unknown", dissect_uaudp_dir_unknown, proto_uaudp);
	register_dissector("uaudp_term_to_serv", dissect_uaudp_term_to_serv, proto_uaudp);
	register_dissector("uaudp_serv_to_term", dissect_uaudp_serv_to_term, proto_uaudp);

	/
	proto_register_field_array(proto_uaudp, hf_uaudp, array_length(hf_uaudp));
	proto_register_subtree_array(ett, array_length(ett));

	/
	uaudp_module = prefs_register_protocol(proto_uaudp, proto_reg_handoff_uaudp);

/
	for (i=0; i<MAX_TERMINAL_PORTS; i++) {
		prefs_register_uint_preference(uaudp_module,
									   ports[i].name,
									   ports[i].text,
									   ports[i].text,
									   10,
									   &ports[i].port);
	}
	prefs_register_string_preference(uaudp_module, "system_ip",
									 "System IP Address (optional)",
									 "IPv4 address of the DHS3 system. (Used only in case of identical source and destination ports)",
									 &pref_sys_ip_s);

	/
/
}
