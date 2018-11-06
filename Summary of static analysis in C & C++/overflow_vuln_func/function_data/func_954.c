void
proto_register_netflow(void)
{
	static hf_register_info hf[] = {
		/
		{&hf_cflow_version,
		 {"Version", "cflow.version",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "NetFlow Version", HFILL}
		 },
		{&hf_cflow_count,
		 {"Count", "cflow.count",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Count of PDUs", HFILL}
		 },
		{&hf_cflow_sysuptime,
		 {"SysUptime", "cflow.sysuptime",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Time since router booted (in milliseconds)", HFILL}
		 },

		{&hf_cflow_timestamp,
		 {"Timestamp", "cflow.timestamp",
		  FT_ABSOLUTE_TIME, BASE_NONE, NULL, 0x0,
		  "Current seconds since epoch", HFILL}
		 },
		{&hf_cflow_unix_secs,
		 {"CurrentSecs", "cflow.unix_secs",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Current seconds since epoch", HFILL}
		 },
		{&hf_cflow_unix_nsecs,
		 {"CurrentNSecs", "cflow.unix_nsecs",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Residual nanoseconds since epoch", HFILL}
		 },
		{&hf_cflow_samplerate,
		 {"SampleRate", "cflow.samplerate",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Sample Frequency of exporter", HFILL}
		 },

		/
		{&hf_cflow_sequence,
		 {"FlowSequence", "cflow.sequence",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Sequence number of flows seen", HFILL}
		 },
		{&hf_cflow_engine_type,
		 {"EngineType", "cflow.engine_type",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "Flow switching engine type", HFILL}
		 },
		{&hf_cflow_engine_id,
		 {"EngineId", "cflow.engine_id",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "Slot number of switching engine", HFILL}
		 },
		{&hf_cflow_source_id,
		 {"SourceId", "cflow.source_id",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Identifier for export device", HFILL}
		 },
		{&hf_cflow_aggmethod,
		 {"AggMethod", "cflow.aggmethod",
		  FT_UINT8, BASE_DEC, VALS(v8_agg), 0x0,
		  "CFlow V8 Aggregation Method", HFILL}
		 },
		{&hf_cflow_aggversion,
		 {"AggVersion", "cflow.aggversion",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "CFlow V8 Aggregation Version", HFILL}
		 },
		/
		/
		{&hf_cflow_flowset_id,
		 {"FlowSet Id", "cflow.flowset_id",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "FlowSet Id", HFILL}
		 },
		{&hf_cflow_data_flowset_id,
		 {"Data FlowSet (Template Id)", "cflow.data_flowset_id",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Data FlowSet with corresponding to a template Id", HFILL}
		 },
		{&hf_cflow_options_flowset_id,
		 {"Options FlowSet", "cflow.options_flowset_id",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Options FlowSet", HFILL}
		 },
		{&hf_cflow_template_flowset_id,
		 {"Template FlowSet", "cflow.template_flowset_id",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Template FlowSet", HFILL}
		 },
		{&hf_cflow_flowset_length,
		 {"FlowSet Length", "cflow.flowset_length",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "FlowSet length", HFILL}
		 },
		{&hf_cflow_template_id,
		 {"Template Id", "cflow.template_id",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Template Id", HFILL}
		 },
		{&hf_cflow_template_field_count,
		 {"Field Count", "cflow.template_field_count",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Template field count", HFILL}
		 },
		{&hf_cflow_template_field_type,
		 {"Type", "cflow.template_field_type",
		  FT_UINT16, BASE_DEC, VALS(v9_template_types), 0x0,
		  "Template field type", HFILL}
		 },
		{&hf_cflow_template_field_length,
		 {"Length", "cflow.template_field_length",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Template field length", HFILL}
		 },
		/
		{&hf_cflow_srcaddr,
		 {"SrcAddr", "cflow.srcaddr",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Flow Source Address", HFILL}
		 },
		{&hf_cflow_srcaddr_v6,
		 {"SrcAddr", "cflow.srcaddrv6",
		  FT_IPv6, BASE_NONE, NULL, 0x0,
		  "Flow Source Address", HFILL}
		 },
		{&hf_cflow_srcnet,
		 {"SrcNet", "cflow.srcnet",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Flow Source Network", HFILL}
		 },
		{&hf_cflow_dstaddr,
		 {"DstAddr", "cflow.dstaddr",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Flow Destination Address", HFILL}
		 },
		{&hf_cflow_dstaddr_v6,
		 {"DstAddr", "cflow.dstaddrv6",
		  FT_IPv6, BASE_NONE, NULL, 0x0,
		  "Flow Destination Address", HFILL}
		 },
		{&hf_cflow_dstnet,
		 {"DstNet", "cflow.dstaddr",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Flow Destination Network", HFILL}
		 },
		{&hf_cflow_nexthop,
		 {"NextHop", "cflow.nexthop",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Router nexthop", HFILL}
		 },
		{&hf_cflow_nexthop_v6,
		 {"NextHop", "cflow.nexthopv6",
		  FT_IPv6, BASE_NONE, NULL, 0x0,
		  "Router nexthop", HFILL}
		 },
		{&hf_cflow_bgpnexthop,
		 {"BGPNextHop", "cflow.bgpnexthop",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "BGP Router Nexthop", HFILL}
		 },
		{&hf_cflow_bgpnexthop_v6,
		 {"BGPNextHop", "cflow.bgpnexthopv6",
		  FT_IPv6, BASE_NONE, NULL, 0x0,
		  "BGP Router Nexthop", HFILL}
		 },
		{&hf_cflow_inputint,
		 {"InputInt", "cflow.inputint",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Flow Input Interface", HFILL}
		 },
		{&hf_cflow_outputint,
		 {"OutputInt", "cflow.outputint",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Flow Output Interface", HFILL}
		 },
		{&hf_cflow_flows,
		 {"Flows", "cflow.flows",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Flows Aggregated in PDU", HFILL}
		 },
		{&hf_cflow_packets,
		 {"Packets", "cflow.packets",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Count of packets", HFILL}
		 },
		{&hf_cflow_packets64,
		 {"Packets", "cflow.packets64",
		  FT_UINT64, BASE_DEC, NULL, 0x0,
		  "Count of packets", HFILL}
		 },
		{&hf_cflow_packetsout,
		 {"PacketsOut", "cflow.packetsout",
		  FT_UINT64, BASE_DEC, NULL, 0x0,
		  "Count of packets going out", HFILL}
		 },
		{&hf_cflow_octets,
		 {"Octets", "cflow.octets",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Count of bytes", HFILL}
		 },
		{&hf_cflow_octets64,
		 {"Octets", "cflow.octets64",
		  FT_UINT64, BASE_DEC, NULL, 0x0,
		  "Count of bytes", HFILL}
		 },
		{&hf_cflow_timestart,
		 {"StartTime", "cflow.timestart",
		  FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
		  "Uptime at start of flow", HFILL}
		 },
		{&hf_cflow_timeend,
		 {"EndTime", "cflow.timeend",
		  FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
		  "Uptime at end of flow", HFILL}
		 },
		{&hf_cflow_srcport,
		 {"SrcPort", "cflow.srcport",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Flow Source Port", HFILL}
		 },
		{&hf_cflow_dstport,
		 {"DstPort", "cflow.dstport",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Flow Destination Port", HFILL}
		 },
		{&hf_cflow_prot,
		 {"Protocol", "cflow.protocol",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "IP Protocol", HFILL}
		 },
		{&hf_cflow_tos,
		 {"IP ToS", "cflow.tos",
		  FT_UINT8, BASE_HEX, NULL, 0x0,
		  "IP Type of Service", HFILL}
		 },
		{&hf_cflow_flags,
		 {"Export Flags", "cflow.flags",
		  FT_UINT8, BASE_HEX, NULL, 0x0,
		  "CFlow Flags", HFILL}
		 },
		{&hf_cflow_tcpflags,
		 {"TCP Flags", "cflow.tcpflags",
		  FT_UINT8, BASE_HEX, NULL, 0x0,
		  "TCP Flags", HFILL}
		 },
		{&hf_cflow_srcas,
		 {"SrcAS", "cflow.srcas",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Source AS", HFILL}
		 },
		{&hf_cflow_dstas,
		 {"DstAS", "cflow.dstas",
		  FT_UINT16, BASE_DEC, NULL, 0x0,
		  "Destination AS", HFILL}
		 },
		{&hf_cflow_srcmask,
		 {"SrcMask", "cflow.srcmask",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "Source Prefix Mask", HFILL}
		 },
		{&hf_cflow_dstmask,
		 {"DstMask", "cflow.dstmask",
		  FT_UINT8, BASE_DEC, NULL, 0x0,
		  "Destination Prefix Mask", HFILL}
		 },
		{&hf_cflow_routersc,
		 {"Router Shortcut", "cflow.routersc",
		  FT_IPv4, BASE_NONE, NULL, 0x0,
		  "Router shortcut by switch", HFILL}
		 },
		{&hf_cflow_mulpackets,
		 {"MulticastPackets", "cflow.mulpackets",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Count of multicast packets", HFILL}
		 },
		{&hf_cflow_muloctets,
		 {"MulticastOctets", "cflow.muloctets",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Count of multicast octets", HFILL}
		 },
		{&hf_cflow_octets_exp,
		 {"OctetsExp", "cflow.octetsexp",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Octets exported", HFILL}
		 },
		{&hf_cflow_packets_exp,
		 {"PacketsExp", "cflow.packetsexp",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Packets exported", HFILL}
		 },
		{&hf_cflow_flows_exp,
		 {"FlowsExp", "cflow.flowsexp",
		  FT_UINT32, BASE_DEC, NULL, 0x0,
		  "Flows exported", HFILL}
		 }
		/
	};

	static gint    *ett[] = {
		&ett_netflow,
		&ett_unixtime,
		&ett_flow,
		&ett_template,
		&ett_dataflowset
	};

	module_t *netflow_module;

	proto_netflow = proto_register_protocol("Cisco NetFlow", "CFLOW",
						"cflow");

	proto_register_field_array(proto_netflow, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	/
	netflow_module = prefs_register_protocol(proto_netflow,
	    proto_reg_handoff_netflow);

	prefs_register_uint_preference(netflow_module, "udp.port",
	    "NetFlow UDP Port", "Set the port for NetFlow messages",
	    10, &global_netflow_udp_port);

	register_dissector("cflow", dissect_netflow, proto_netflow);
}
