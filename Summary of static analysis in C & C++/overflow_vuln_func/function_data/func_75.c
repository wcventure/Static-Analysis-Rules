void proto_register_ua3g(void)
{
	static hf_register_info hf_ua3g[] =
		{
			{ &hf_ua3g_length,
			  { "Length", "ua3g.length", FT_UINT16, BASE_DEC,
			    NULL, 0x00, "Decimal Value", HFILL }
			},
			{ &hf_ua3g_opcode,
			  { "Opcode", "ua3g.opcode", FT_UINT8, BASE_HEX,
			    NULL, 0x00, "Hexa Value", HFILL }
			},
			{ &hf_ua3g_ip,
			  { "IP Device Routing", "ua3g.ip", FT_UINT8, BASE_HEX,
			    NULL, 0x00, "Hexa Value - 2nd Command For IP Device Routing Opcode", HFILL }
			},
			{ &hf_ua3g_command,
			  { "Command", "ua3g.command", FT_UINT8, BASE_HEX,
				NULL, 0x00, "Hexa Value - 2nd Command (Excepted IP Device Routing Opcode)", HFILL }
			},
		};

	static gint *ett[] =
	{
		&ett_ua3g,
		&ett_ua3g_body,
		&ett_ua3g_param,
		&ett_ua3g_option,
	};

	/
	proto_ua3g = proto_register_protocol("UA3G Message", "UA3G", "ua3g");

	proto_register_field_array(proto_ua3g, hf_ua3g, array_length(hf_ua3g));

	register_dissector("ua3g", dissect_ua3g, proto_ua3g);

	/
	proto_register_subtree_array(ett, array_length(ett));
}
