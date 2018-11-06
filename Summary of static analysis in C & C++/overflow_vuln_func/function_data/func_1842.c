void proto_register_gnutella(void) {

	static hf_register_info hf[] = {
		{ &hf_gnutella_header,
			{ "Descriptor Header", "gnutella.header",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella Descriptor Header", HFILL }
		},
		{ &hf_gnutella_pong_payload,
			{ "Pong", "gnutella.pong.payload",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella Pong Payload", HFILL }
		},
		{ &hf_gnutella_push_payload,
			{ "Push", "gnutella.push.payload",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella Push Payload", HFILL }
		},
		{ &hf_gnutella_query_payload,
			{ "Query", "gnutella.query.payload",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella Query Payload", HFILL }
		},
		{ &hf_gnutella_queryhit_payload,
			{ "QueryHit", "gnutella.queryhit.payload",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella QueryHit Payload", HFILL }
		},
		{ &hf_gnutella_truncated,
			{ "Truncated Frame", "gnutella.truncated",
			FT_NONE, BASE_NONE, NULL, 0,
			"The Gnutella Frame Was Truncated", HFILL }
		},
		{ &hf_gnutella_stream,
			{ "Gnutella Upload / Download Stream", "gnutella.stream",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella Upload / Download Stream", HFILL }
		},
		{ &hf_gnutella_header_id,
			{ "ID", "gnutella.header.id",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Gnutella Descriptor ID", HFILL }
		},
		{ &hf_gnutella_header_payload,
			{ "Payload", "gnutella.header.payload",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Gnutella Descriptor Payload", HFILL }
		},
		{ &hf_gnutella_header_ttl,
			{ "TTL", "gnutella.header.ttl",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Gnutella Descriptor Time To Live", HFILL }
		},
		{ &hf_gnutella_header_hops,
			{ "Hops", "gnutella.header.hops",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Gnutella Descriptor Hop Count", HFILL }
		},
		{ &hf_gnutella_header_size,
			{ "Length", "gnutella.header.size",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Gnutella Descriptor Payload Length", HFILL }
		},
		{ &hf_gnutella_pong_port,
			{ "Port", "gnutella.pong.port",
			FT_UINT16, BASE_DEC, NULL, 0,
			"Gnutella Pong TCP Port", HFILL }
		},
		{ &hf_gnutella_pong_ip,
			{ "IP", "gnutella.pong.ip",
			FT_IPv4, BASE_DEC, NULL, 0,
			"Gnutella Pong IP Address", HFILL }
		},
		{ &hf_gnutella_pong_files,
			{ "Files Shared", "gnutella.pong.files",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella Pong Files Shared", HFILL }
		},
		{ &hf_gnutella_pong_kbytes,
			{ "KBytes Shared", "gnutella.pong.kbytes",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella Pong KBytes Shared", HFILL }
		},
		{ &hf_gnutella_query_min_speed,
			{ "Min Speed", "gnutella.query.min_speed",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella Query Minimum Speed", HFILL }
		},
		{ &hf_gnutella_query_search,
			{ "Search", "gnutella.query.search",
			FT_STRINGZ, BASE_NONE, NULL, 0,
			"Gnutella Query Search", HFILL }
		},
		{ &hf_gnutella_queryhit_hit,
			{ "Hit", "gnutella.queryhit.hit",
			FT_NONE, BASE_NONE, NULL, 0,
			"Gnutella QueryHit", HFILL }
		},
		{ &hf_gnutella_queryhit_hit_index,
			{ "Index", "gnutella.queryhit.hit.index",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella QueryHit Index", HFILL }
		},
		{ &hf_gnutella_queryhit_hit_size,
			{ "Size", "gnutella.queryhit.hit.size",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella QueryHit Size", HFILL }
		},
		{ &hf_gnutella_queryhit_hit_name,
			{ "Name", "gnutella.queryhit.hit.name",
			FT_STRING, BASE_NONE, NULL, 0,
			"Gnutella Query Name", HFILL }
		},
		{ &hf_gnutella_queryhit_hit_extra,
			{ "Extra", "gnutella.queryhit.hit.extra",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Gnutella Query Extra", HFILL }
		},
		{ &hf_gnutella_queryhit_count,
			{ "Count", "gnutella.queryhit.count",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Gnutella QueryHit Count", HFILL }
		},
		{ &hf_gnutella_queryhit_port,
			{ "Port", "gnutella.queryhit.port",
			FT_UINT16, BASE_DEC, NULL, 0,
			"Gnutella QueryHit Port", HFILL }
		},
		{ &hf_gnutella_queryhit_ip,
			{ "IP", "gnutella.queryhit.ip",
			FT_IPv4, BASE_DEC, NULL, 0,
			"Gnutella QueryHit IP Address", HFILL }
		},
		{ &hf_gnutella_queryhit_speed,
			{ "Speed", "gnutella.queryhit.speed",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella QueryHit Speed", HFILL }
		},
		{ &hf_gnutella_queryhit_extra,
			{ "Extra", "gnutella.queryhit.extra",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Gnutella QueryHit Extra", HFILL }
		},
		{ &hf_gnutella_queryhit_servent_id,
			{ "Servent ID", "gnutella.queryhit.servent_id",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Gnutella QueryHit Servent ID", HFILL }
		},
		{ &hf_gnutella_push_servent_id,
			{ "Servent ID", "gnutella.push.servent_id",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Gnutella Push Servent ID", HFILL }
		},
		{ &hf_gnutella_push_ip,
			{ "IP", "gnutella.push.ip",
			FT_IPv4, BASE_DEC, NULL, 0,
			"Gnutella Push IP Address", HFILL }
		},
		{ &hf_gnutella_push_index,
			{ "Index", "gnutella.push.index",
			FT_UINT32, BASE_DEC, NULL, 0,
			"Gnutella Push Index", HFILL }
		},
		{ &hf_gnutella_push_port,
			{ "Port", "gnutella.push.port",
			FT_UINT16, BASE_DEC, NULL, 0,
			"Gnutella Push Port", HFILL }
		},
	};

	static gint *ett[] = {
		&ett_gnutella,
	};

	proto_gnutella = proto_register_protocol("Gnutella Protocol",
	    					"GNUTELLA",
						"gnutella");

	proto_register_field_array(proto_gnutella, hf, array_length(hf));

	proto_register_subtree_array(ett, array_length(ett));
}
