void
proto_register_bacapp(void)
{
	static hf_register_info hf[] = {
		{ &hf_bacapp_type,
			{ "APDU Type",           "bacapp.bacapp_type",
			FT_UINT8, BASE_DEC, VALS(bacapp_type_name), 0xf0, "APDU Type", HFILL }
		},
		{ &hf_bacapp_SEG,
			{ "SEG",           "bacapp.bacapp_type.SEG",
			FT_BOOLEAN, 8, TFS(&segments_follow), 0x08, "Segmented Requests", HFILL }
		},
		{ &hf_bacapp_MOR,
			{ "MOR",           "bacapp.bacapp_type.MOR",
			FT_BOOLEAN, 8, TFS(&more_follow), 0x04, "More Segments Follow", HFILL }
		},
		{ &hf_bacapp_SA,
			{ "SA",           "bacapp.bacapp_type.SA",
			FT_BOOLEAN, 8, TFS(&segmented_accept), 0x02, "Segmented Response accepted", HFILL }
		},
		{ &hf_bacapp_max_adpu_size,
			{ "Size of Maximum ADPU accepted",           "bacapp.bacapp_max_adpu_size",
			FT_UINT8, BASE_DEC, VALS(bacapp_max_APDU_length_accepted), 0x0f, "Size of Maximum ADPU accepted", HFILL }
		},
		{ &hf_bacapp_response_segments,
			{ "Max Response Segments accepted",           "bacapp.bacapp_response_segments",
			FT_UINT8, BASE_DEC, VALS(bacapp_max_segments_accepted), 0xe0, "Max Response Segments accepted", HFILL }
		},
		{ &hf_bacapp_invoke_id,
			{ "Invoke ID",           "bacapp.bacapp_invoke_id",
			FT_UINT8, BASE_HEX, NULL, 0, "Invoke ID", HFILL }
		},
		{ &hf_bacapp_sequence_number,
			{ "Sequence Number",           "bacapp.bacapp_sequence_number",
			FT_UINT8, BASE_DEC, NULL, 0, "Sequence Number", HFILL }
		},
		{ &hf_bacapp_window_size,
			{ "Proposed Window Size",           "bacapp.bacapp_window_size",
			FT_UINT8, BASE_DEC, NULL, 0, "Proposed Window Size", HFILL }
		},
		{ &hf_bacapp_service,
			{ "Service Choice",           "bacapp.bacapp_service",
			FT_UINT8, BASE_DEC, VALS(bacapp_confirmed_service_choice), 0x00, "Service Choice", HFILL }
		},
		{ &hf_bacapp_uservice,
			{ "Unconfirmed Service Choice",           "bacapp.bacapp_unconfirmed_service",
			FT_UINT8, BASE_DEC, VALS(BACnetUnconfirmedServiceChoice), 0x00, "Unconfirmed Service Choice", HFILL }
		},
		{ &hf_bacapp_NAK,
			{ "NAK",           "bacapp.bacapp_type.NAK",
			FT_BOOLEAN, 8, NULL, 0x02, "negativ ACK", HFILL }
		},
		{ &hf_bacapp_SRV,
			{ "SRV",           "bacapp.bacapp_type.SRV",
			FT_BOOLEAN, 8, NULL, 0x01, "Server", HFILL }
		},
		{ &hf_bacapp_reject_reason,
			{ "Reject Reason",           "bacapp.bacapp_reject_reason",
			FT_UINT8, BASE_DEC, VALS(bacapp_reject_reason), 0x00, "Reject Reason", HFILL }
		},
		{ &hf_bacapp_abort_reason,
			{ "Abort Reason",           "bacapp.bacapp_abort_reason",
			FT_UINT8, BASE_DEC, VALS(bacapp_abort_reason), 0x00, "Abort Reason", HFILL }
		},
		{ &hf_bacapp_vpart,
			{ "BACnet APDU variable part:",           "bacapp.variable_part",
			FT_NONE, 0, NULL, 00, "BACnet APDU varaiable part:", HFILL }
		},
		{ &hf_bacapp_tag_number,
			{ "Tag Number",           "bacapp.bacapp_tag.number",
			FT_UINT8, BASE_DEC, VALS(bacapp_tag_number), 0xF0, "Tag Number", HFILL }
		},
		{ &hf_bacapp_tag_class,
			{ "Class",           "bacapp.bacapp_tag.class",
			FT_BOOLEAN, 8, TFS(&bacapp_tag_class), 0x08, "Class", HFILL }
		},
		{ &hf_bacapp_tag_lvt,
			{ "Length Value Type",           "bacapp.bacapp_tag.lvt",
			FT_UINT8, BASE_DEC, NULL, 0x07, "Length Value Type", HFILL }
		},
/		{ &hf_bacapp_tag_ProcessId,
			{ "subscriberProcessIdentifier",           "bacapp.bacapp_tag.ProcessId",
			FT_UINT32, BASE_DEC, NULL, 0, "subscriberProcessIdentifier", HFILL }
		},
		{ &hf_bacapp_tag_initiatingObjectType,
			{ "ObjectType",           "bacapp.bacapp_tag.ObjectType",
			FT_UINT16, BASE_DEC, VALS(bacapp_object_type), 0x00, "ObjectType", HFILL }
		},
/
	};
	static gint *ett[] = {
		&ett_bacapp,
		&ett_bacapp_control,
		&ett_bacapp_tag,
	};
	proto_bacapp = proto_register_protocol("Building Automation and Control Network APDU",
	    "BACapp", "bacapp");

	proto_register_field_array(proto_bacapp, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
	register_dissector("bacapp", dissect_bacapp, proto_bacapp);
}
