void proto_register_a11(void)
{                 

/
	static hf_register_info hf[] = {
	  { &hf_a11_type,
		 { "Message Type",           "a11.type",
			FT_UINT8, BASE_DEC, VALS(a11_types), 0,          
			"A11 Message type.", HFILL }
	  },
	  { &hf_a11_flags,
		{"Flags", "a11.flags",
		 FT_UINT8, BASE_HEX, NULL, 0x0,
		 "", HFILL}
	  },
	  { &hf_a11_s,
		 {"Simultaneous Bindings",           "a11.s",

		   FT_BOOLEAN, 8, NULL, 128,          
		   "Simultaneous Bindings Allowed", HFILL }
	  },
	  { &hf_a11_b,
		 {"Broadcast Datagrams",           "a11.b",
		   FT_BOOLEAN, 8, NULL, 64,          
		   "Broadcast Datagrams requested", HFILL }
	  },
	  { &hf_a11_d,
		 { "Co-located Care-of Address",           "a11.d",
		   FT_BOOLEAN, 8, NULL, 32,          
		   "MN using Co-located Care-of address", HFILL }
	  },
	  { &hf_a11_m,
		 {"Minimal Encapsulation",           "a11.m",
		   FT_BOOLEAN, 8, NULL, 16,          
		   "MN wants Minimal encapsulation", HFILL }
	  },
	  { &hf_a11_g,
		 {"GRE",           "a11.g",
		   FT_BOOLEAN, 8, NULL, 8,          
		   "MN wants GRE encapsulation", HFILL }
	  },
	  { &hf_a11_v,
		 { "Van Jacobson",           "a11.v",
		   FT_BOOLEAN, 8, NULL, 4,          
		   "Van Jacobson", HFILL }
	  },
	  { &hf_a11_t,
		 { "Reverse Tunneling",           "a11.t",
		   FT_BOOLEAN, 8, NULL, 2,          
		   "Reverse tunneling requested", HFILL }
	  },
	  { &hf_a11_code,
		 { "Reply Code",           "a11.code",
			FT_UINT8, BASE_DEC, VALS(a11_reply_codes), 0,          
			"A11 Registration Reply code.", HFILL }
	  },
	  { &hf_a11_status,
		 { "Reply Status",           "a11.ackstat",
			FT_UINT8, BASE_DEC, VALS(a11_ack_status), 0,          
			"A11 Registration Ack Status.", HFILL }
	  },
	  { &hf_a11_life,
		 { "Lifetime",           "a11.life",
			FT_UINT16, BASE_DEC, NULL, 0,          
			"A11 Registration Lifetime.", HFILL }
	  },
	  { &hf_a11_homeaddr,
		 { "Home Address",           "a11.homeaddr",
			FT_IPv4, BASE_NONE, NULL, 0,          
			"Mobile Node's home address.", HFILL }
	  },
	  
	  { &hf_a11_haaddr,
		 { "Home Agent",           "a11.haaddr",
			FT_IPv4, BASE_NONE, NULL, 0,          
			"Home agent IP Address.", HFILL }
	  },
	  { &hf_a11_coa,
		 { "Care of Address",           "a11.coa",
			FT_IPv4, BASE_NONE, NULL, 0,          
			"Care of Address.", HFILL }
	  },
	  { &hf_a11_ident,
		 { "Identification",           "a11.ident",
			FT_BYTES, BASE_NONE, NULL, 0,          
			"MN Identification.", HFILL }
	  },
	  { &hf_a11_ext_type,
		 { "Extension Type",           "a11.ext.type",
			FT_UINT8, BASE_DEC, VALS(a11_ext_types), 0,          
			"Mobile IP Extension Type.", HFILL }
	  },
	  { &hf_a11_ext_stype,
		 { "Gen Auth Ext SubType",           "a11.ext.auth.subtype",
			FT_UINT8, BASE_DEC, VALS(a11_ext_stypes), 0,          
			"Mobile IP Auth Extension Sub Type.", HFILL }
	  },
	  { &hf_a11_ext_len,
		 { "Extension Length",         "a11.ext.len",
			FT_UINT16, BASE_DEC, NULL, 0,
			"Mobile IP Extension Length.", HFILL }
	  },
	  { &hf_a11_ext,
		 { "Extension",                      "a11.extension",
			FT_BYTES, BASE_HEX, NULL, 0,
			"Extension", HFILL }
	  },
	  { &hf_a11_aext_spi,
		 { "SPI",                      "a11.auth.spi",
			FT_UINT32, BASE_HEX, NULL, 0,
			"Authentication Header Security Parameter Index.", HFILL }
	  },
	  { &hf_a11_aext_auth,
		 { "Authenticator",            "a11.auth.auth",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Authenticator.", HFILL }
	  },
	  { &hf_a11_next_nai,
		 { "NAI",                      "a11.nai",
			FT_STRING, BASE_NONE, NULL, 0,
			"NAI", HFILL }
	  },
	  { &hf_a11_ses_key,
		 { "Key",                      "a11.ext.key",
			FT_UINT32, BASE_HEX, NULL, 0,
			"Session Key.", HFILL }
	  },
	  { &hf_a11_ses_sidver,
		{ "Session ID Version",         "a11.ext.sidver",
			FT_UINT8, BASE_DEC, NULL, 3,
			"Session ID Version", HFILL}
	  },
	  { &hf_a11_ses_mnsrid,
		 { "MNSR-ID",                      "a11.ext.mnsrid",
			FT_UINT16, BASE_HEX, NULL, 0,
			"MNSR-ID", HFILL }
	  },
	  { &hf_a11_ses_msid_type,
		 { "MSID Type",                      "a11.ext.msid_type",
			FT_UINT16, BASE_DEC, NULL, 0,
			"MSID Type.", HFILL }
	  },
	  { &hf_a11_ses_msid_len,
		 { "MSID Length",                      "a11.ext.msid_len",
			FT_UINT8, BASE_DEC, NULL, 0,
			"MSID Length.", HFILL }
	  },
	  { &hf_a11_ses_msid,
		 { "MSID(BCD)",                      "a11.ext.msid",
			FT_STRING, BASE_NONE, NULL, 0,
			"MSID(BCD).", HFILL }
	  },
	  { &hf_a11_ses_ptype,
		 { "Protocol Type",                      "a11.ext.ptype",
			FT_UINT16, BASE_HEX, VALS(a11_ses_ptype_vals), 0,
			"Protocol Type.", HFILL }
	  },
	  { &hf_a11_vse_vid,
		 { "Vendor ID",                      "a11.ext.vid",
			FT_UINT32, BASE_HEX, VALS(sminmpec_values), 0,
			"Vendor ID.", HFILL }
	  },
	  { &hf_a11_vse_apptype,
		 { "Application Type",                      "a11.ext.apptype",
			FT_UINT8, BASE_HEX, VALS(a11_ext_app), 0,
			"Application Type.", HFILL }
	  },
	  { &hf_a11_vse_ppaddr,
		 { "Anchor P-P Address",           "a11.ext.ppaddr",
			FT_IPv4, BASE_NONE, NULL, 0,          
			"Anchor P-P Address.", HFILL }
	  },
	  { &hf_a11_vse_dormant,
		 { "All Dormant Indicator",           "a11.ext.dormant",
			FT_UINT16, BASE_HEX, VALS(a11_ext_dormant), 0,          
			"All Dormant Indicator.", HFILL }
	  },
	  { &hf_a11_vse_code,
		 { "Reply Code",           "a11.ext.code",
			FT_UINT8, BASE_DEC, VALS(a11_reply_codes), 0,          
			"PDSN Code.", HFILL }
	  },
	  { &hf_a11_vse_pdit,
		 { "PDSN Code",                      "a11.ext.code",
			FT_UINT8, BASE_HEX, VALS(a11_ext_nvose_pdsn_code), 0,
			"PDSN Code.", HFILL }
	  },
	  { &hf_a11_vse_srvopt,
		 { "Service Option",                      "a11.ext.srvopt",
			FT_UINT16, BASE_HEX, VALS(a11_ext_nvose_srvopt), 0,
			"Service Option.", HFILL }
	  },
	  { &hf_a11_vse_panid,
		 { "PANID",                      "a11.ext.panid",
			FT_BYTES, BASE_HEX, NULL, 0,
			"PANID", HFILL }
	  },
	  { &hf_a11_vse_canid,
		 { "CANID",                      "a11.ext.canid",
			FT_BYTES, BASE_HEX, NULL, 0,
			"CANID", HFILL }
	  },
	  { &hf_a11_vse_qosmode,
		 { "QoS Mode",       "a11.ext.qosmode",
			FT_UINT8, BASE_HEX, VALS(a11_ext_nvose_qosmode), 0,
			"QoS Mode.", HFILL }
      },
      { &hf_a11_ase_len_type,
		 { "Entry Length",   "a11.ext.ase.len",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Entry Length.", HFILL }
      },
      { &hf_a11_ase_srid_type,
		 { "Service Reference ID (SRID)",   "a11.ext.ase.srid",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Service Reference ID (SRID).", HFILL }
      },
      { &hf_a11_ase_servopt_type,
		 { "Service Option", "a11.ext.ase.srvopt",
			FT_UINT16, BASE_HEX, VALS(a11_ext_nvose_srvopt), 0,
			"Service Option.", HFILL }
      },
	  { &hf_a11_ase_gre_proto_type,
		 { "GRE Protocol Type",   "a11.ext.ase.ptype",
			FT_UINT16, BASE_HEX, VALS(a11_ses_ptype_vals), 0,
			"GRE Protocol Type.", HFILL }
	  },
	  { &hf_a11_ase_gre_key,
		 { "GRE Key",   "a11.ext.ase.key",
			FT_UINT32, BASE_HEX, NULL, 0,
			"GRE Key.", HFILL }
	  },
	  { &hf_a11_ase_pcf_addr_key,
		 { "PCF IP Address",           "a11.ext.ase.pcfip",
			FT_IPv4, BASE_NONE, NULL, 0,
			"PCF IP Address.", HFILL }
	  },
	  { &hf_a11_fqi_srid,
		 { "SRID",   "a11.ext.fqi.srid",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Flow Entry SRID.", HFILL }
	  },
	  { &hf_a11_fqi_flags,
		 { "Flags",   "a11.ext.fqi.flags",
			FT_UINT8, BASE_HEX, NULL, 0,
			"Forward Flow Entry Flags.", HFILL }
	  },
	  { &hf_a11_fqi_flowcount,
		 { "Forward Flow Count",   "a11.ext.fqi.flowcount",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Flow Count.", HFILL }
	  },
	  { &hf_a11_fqi_flowid,
		 { "Forward Flow Id",   "a11.ext.fqi.flowid",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Flow Id.", HFILL }
	  },
	  { &hf_a11_fqi_entrylen,
		 { "Entry Length",   "a11.ext.fqi.entrylen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Entry Length.", HFILL }
	  },
	  { &hf_a11_fqi_dscp,
		 { "Forward DSCP",   "a11.ext.fqi.dscp",
			FT_UINT8, BASE_HEX, NULL, 0,
			"Forward Flow DSCP.", HFILL }
	  },
	  { &hf_a11_fqi_flowstate,
		 { "Forward Flow State",   "a11.ext.fqi.flowstate",
			FT_UINT8, BASE_HEX, NULL, 0,
			"Forward Flow State.", HFILL }
	  },
	  { &hf_a11_fqi_requested_qoslen,
		 { "Requested QoS Length",   "a11.ext.fqi.reqqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Requested QoS Length.", HFILL }
	  },
	  { &hf_a11_fqi_requested_qos,
		 { "Requested QoS",   "a11.ext.fqi.reqqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Forward Requested QoS.", HFILL }
	  },
	  { &hf_a11_fqi_granted_qoslen,
		 { "Granted QoS Length",   "a11.ext.fqi.graqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward Granted QoS Length.", HFILL }
	  },
	  { &hf_a11_fqi_granted_qos,
		 { "Granted QoS",   "a11.ext.fqi.graqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Forward Granted QoS.", HFILL }
	  },
	  { &hf_a11_rqi_srid,
		 { "SRID",   "a11.ext.rqi.srid",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Flow Entry SRID.", HFILL }
	  },
	  { &hf_a11_rqi_flowcount,
		 { "Reverse Flow Count",   "a11.ext.rqi.flowcount",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Flow Count.", HFILL }
	  },
	  { &hf_a11_rqi_flowid,
		 { "Reverse Flow Id",   "a11.ext.rqi.flowid",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Flow Id.", HFILL }
	  },
	  { &hf_a11_rqi_entrylen,
		 { "Entry Length",   "a11.ext.rqi.entrylen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Flow Entry Length.", HFILL }
	  },
	  { &hf_a11_rqi_flowstate,
		 { "Flow State",   "a11.ext.rqi.flowstate",
			FT_UINT8, BASE_HEX, NULL, 0,
			"Reverse Flow State.", HFILL }
	  },
	  { &hf_a11_rqi_requested_qoslen,
		 { "Requested QoS Length",   "a11.ext.rqi.reqqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Requested QoS Length.", HFILL }
	  },
	  { &hf_a11_rqi_requested_qos,
		 { "Requested QoS",   "a11.ext.rqi.reqqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Reverse Requested QoS.", HFILL }
	  },
	  { &hf_a11_rqi_granted_qoslen,
		 { "Granted QoS Length",   "a11.ext.rqi.graqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Granted QoS Length.", HFILL }
	  },
	  { &hf_a11_rqi_granted_qos,
		 { "Granted QoS",   "a11.ext.rqi.graqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Reverse Granted QoS.", HFILL }
	  },
	  { &hf_a11_fqui_flowcount,
		 { "Forward QoS Update Flow Count",   "a11.ext.fqui.flowcount",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Forward QoS Update Flow Count.", HFILL }
	  },
	  { &hf_a11_rqui_flowcount,
		 { "Reverse QoS Update Flow Count",   "a11.ext.rqui.flowcount",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse QoS Update Flow Count.", HFILL }
	  },
	  { &hf_a11_fqui_updated_qoslen,
		 { "Foward Updated QoS Sub-Blob Length",   "a11.ext.fqui.updatedqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Foward Updated QoS Sub-Blob Length.", HFILL }
	  },
	  { &hf_a11_fqui_updated_qos,
		 { "Foward Updated QoS Sub-Blob",   "a11.ext.fqui.updatedqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Foward Updated QoS Sub-Blob.", HFILL }
	  },
	  { &hf_a11_rqui_updated_qoslen,
		 { "Reverse Updated QoS Sub-Blob Length",   "a11.ext.rqui.updatedqoslen",
			FT_UINT8, BASE_DEC, NULL, 0,
			"Reverse Updated QoS Sub-Blob Length.", HFILL }
	  },
	  { &hf_a11_rqui_updated_qos,
		 { "Reverse Updated QoS Sub-Blob",   "a11.ext.rqui.updatedqos",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Reverse Updated QoS Sub-Blob.", HFILL }
	  },
	  { & hf_a11_subsciber_profile_len,
		 { "Subscriber QoS Profile Length",   "a11.ext.sqp.profilelen",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Subscriber QoS Profile Length.", HFILL }
	  },
	  { & hf_a11_subsciber_profile,
		 { "Subscriber QoS Profile",   "a11.ext.sqp.profile",
			FT_BYTES, BASE_NONE, NULL, 0,
			"Subscriber QoS Profile.", HFILL }
	  },
	};

	/
	static gint *ett[] = {
		&ett_a11,
		&ett_a11_flags,
		&ett_a11_ext,
		&ett_a11_exts,
		&ett_a11_radius,
		&ett_a11_radiuses,
		&ett_a11_ase,
		&ett_a11_fqi_flowentry,
		&ett_a11_rqi_flowentry,
		&ett_a11_fqi_flags,
		&ett_a11_fqi_entry_flags,
		&ett_a11_rqi_entry_flags,
		&ett_a11_fqui_flowentry,
		&ett_a11_rqui_flowentry,
		&ett_a11_subscriber_profile,
	};

	/
	proto_a11 = proto_register_protocol("3GPP2 A11", "3GPP2 A11", "a11");

	/
	new_register_dissector("a11", dissect_a11, proto_a11);

	/
	proto_register_field_array(proto_a11, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
}
