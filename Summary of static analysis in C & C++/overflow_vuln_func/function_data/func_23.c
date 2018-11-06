void proto_register_noe(void)
{
	static hf_register_info hf_noe[] =
	{
		{ &hf_noe_length,
		{
			"Length",
			"noe.length",
			FT_UINT16,
			BASE_DEC,
			NULL,
			0x0,
			"Method Length",
			HFILL
		}
		},  
		{ &hf_noe_server,
		{
			"Server",
			"noe.server",
			FT_UINT8,
			BASE_HEX,
			VALS(servers_vals),
			0x0,
			"Method Opcode",
			HFILL
		}
		},  
		{ &hf_noe_method_ack,
		{
			"Ack",
			"noe.method_ack",
			FT_BOOLEAN,
			BASE_NONE,
			NULL,
			0x0,
			"Method Acknowledge",
			HFILL
		}
		},
		{ &hf_noe_method,
		{
			"Method",
			"noe.method",
			FT_UINT8,
			BASE_DEC,
			VALS(methods_vals),
			0x0,
			"Method Opcode",
			HFILL
		}
		},
		{ &hf_noe_class,
		{
			"Class",
			"noe.class",
			FT_INT8,
			BASE_DEC,
			NULL,
			0x0,
			"Class Opcode",
			HFILL
		}
		},
		{ &hf_noe_event,
		{
			"Event",
			"noe.event",
			FT_UINT8,
			BASE_DEC,
			NULL,
			0x0,
			"Event Opcode",
			HFILL
		}
		},
		{ &hf_noe_objectid,
		{
			"Objectid",
			"noe.objectid",
			FT_UINT16,
			BASE_HEX,
			NULL,
			0x0,
			"Object Identifier",
			HFILL
		}
		},
		{ &hf_noe_method_index,
		{
			"ItemIndx",
			"noe.item_index",
			FT_UINT8,
			BASE_DEC,
			NULL,
			0x0,
			"Delete/Insert Index",
			HFILL
		}
		},
		{ &hf_noe_pcode,
		{
			"Property",
			"noe.property",
			FT_INT8,
			BASE_DEC,
			NULL,
			0x0,
			"Property Identifier",
			HFILL
		}
		},
		{ &hf_noe_psize,
		{
			"PropLength",
			"noe.prop_len",
			FT_UINT16,
			BASE_DEC,
			NULL,
			0x0,
			"Property Length",
			HFILL
		}
		},
		{ &hf_noe_errcode,
		{
			"ErrCode",
			"noe.errcode",
			FT_UINT16,
			BASE_DEC,
			NULL,
			0x0,
			"Error Code",
			HFILL
		}
		},
		{ &hf_noe_aindx,
		{
			"ArrIndex",
			"noe.array_index",
			FT_UINT8,
			BASE_DEC,
			NULL,
			0x0,
			"Array Index",
			HFILL
		}
		}
	};

	static gint *ett[] =
	{
		&ett_noe,
		&ett_body,
		&ett_property,
		&ett_value,
	};

	/
	proto_noe = proto_register_protocol("NOE Protocol", "NOE", "noe");

	proto_register_field_array(proto_noe, hf_noe, array_length(hf_noe));

	register_dissector("noe", dissect_noe, proto_noe);

	/
	proto_register_subtree_array(ett, array_length(ett));
}
