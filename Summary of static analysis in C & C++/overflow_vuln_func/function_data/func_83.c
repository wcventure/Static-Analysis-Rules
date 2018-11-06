void proto_register_uasip(void)
{
	module_t *uasip_module;

	static hf_register_info hf_uasip[] =
	{
		{ 
			&hf_uasip_opcode,
			{
				"Opcode",
				"uasip.opcode",
				FT_UINT8,
				BASE_DEC,
				VALS(uasip_opcode_str),
				0x0,
				"UA/SIP Opcode",
				HFILL
			}
		},
		{ 
			&hf_uasip_version,
			{
				"Version",
				"uasip.version",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Version",
				HFILL
			}
		},
		{ 
			&hf_uasip_window_size,
			{
				"Window Size",
				"uasip.window_size", 
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Window Size",
				HFILL
			}
		},
		{
			&hf_uasip_mtu,
			{
				"MTU", 
				"uasip.mtu",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP MTU",
				HFILL
			}
		},
		{ 
			&hf_uasip_udp_lost,
			{
				"UDP Lost",
				"uasip.udp_lost",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Lost",
				HFILL
			}
		},
		{ 
			&hf_uasip_udp_lost_reinit,
			{ 
				"UDP lost reinit",
				"uasip.udp_lost_reinit",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Lost Re-Init",
				HFILL
			}
		},
		{ 
			&hf_uasip_keepalive,
			{ 
				"Keepalive",
				"uasip.keepalive",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Keepalive",
				HFILL
			}
		},
		{ 
			&hf_uasip_qos_ip_tos,
			{
				"QoS IP TOS",
				"uasip.qos_ip_tos",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP QoS IP TOS",
				HFILL
			}
		},
		{ 
			&hf_uasip_qos_8021_vlid,
			{ 
				"QoS 802.1 VLID",
				"uasip.qos_8021_vlid",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP QoS 802.1 VLID",
				HFILL
			}
		},
		{ 
			&hf_uasip_qos_8021_pri,
			{ 
				"QoS 802.1 PRI",
				"uasip.qos_8021_pri",
				FT_UINT8,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP QoS 802.1 PRI",
				HFILL
			}
		},
		{ 
			&hf_uasip_expseq,
			{
				"Sequence Number (expected)",
				"uasip.expseq",
				FT_UINT16,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Expected Sequence Number",
				HFILL
			}
		},
		{ 
			&hf_uasip_sntseq,
			{ 
				"Sequence Number (sent)",
				"uasip.sntseq",
				FT_UINT16,
				BASE_DEC,
				NULL,
				0x0,
				"UA/SIP Sent Sequence Number",
				HFILL
			}
		},
	};

	static gint *ett[] =
	{
		&ett_uasip,
	};

	proto_uasip = proto_register_protocol("UA/SIP Protocol", "UASIP", "uasip"); 
	register_dissector("uasip", dissect_uasip, proto_uasip);

	proto_register_field_array(proto_uasip, hf_uasip, array_length(hf_uasip));
	proto_register_subtree_array(ett, array_length(ett));

	uasip_module = prefs_register_protocol(proto_uasip, proto_reg_handoff_uasip);
	prefs_register_bool_preference(uasip_module, "noesip", "Try to decode SIP NOE", "NOE SIP Protocol", &noesip_enabled);
	prefs_register_string_preference(uasip_module, "proxy_ipaddr", "Proxy IP Address", "IPv4 address of the proxy", &pref_proxy_ipaddr_s);
/
}
