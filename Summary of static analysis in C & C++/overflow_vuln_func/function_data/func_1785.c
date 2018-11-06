void
proto_register_snmp(void)
{
#if defined(HAVE_UCD_SNMP_SNMP_H) && defined(linux)
	void *libsnmp_handle;
	int (*snmp_set_suffix_only_p)(int);
	int (*ds_set_int_p)(int, int, int);
#endif

        static hf_register_info hf[] = {
		{ &hf_snmpv3_flags,
		{ "SNMPv3 Flags", "snmpv3.flags", FT_UINT8, BASE_HEX, NULL,
		    0x0, "", HFILL }},
		{ &hf_snmpv3_flags_auth,
		{ "Authenticated", "snmpv3.flags.auth", FT_BOOLEAN, 8,
		    TFS(&flags_set_truth), TH_AUTH, "", HFILL }},
		{ &hf_snmpv3_flags_crypt,
		{ "Encrypted", "snmpv3.flags.crypt", FT_BOOLEAN, 8,
		    TFS(&flags_set_truth), TH_CRYPT, "", HFILL }},
		{ &hf_snmpv3_flags_report,
		{ "Reportable", "snmpv3.flags.report", FT_BOOLEAN, 8,
		    TFS(&flags_set_truth), TH_REPORT, "", HFILL }},
        };
	static gint *ett[] = {
		&ett_snmp,
		&ett_smux,
		&ett_parameters,
		&ett_parameters_qos,
		&ett_global,
		&ett_flags,
		&ett_secur,
	};

#if defined(HAVE_UCD_SNMP_SNMP_H) || defined(HAVE_SNMP_SNMP_H)
	/
	init_mib();
#ifdef HAVE_UCD_SNMP_SNMP_H
#ifdef linux
	/

	libsnmp_handle = dlopen("libsnmp.so.0", RTLD_LAZY|RTLD_GLOBAL);
	if (libsnmp_handle == NULL) {
		/
		snmp_set_suffix_only(2);
	} else {
		/
		snmp_set_suffix_only_p = dlsym(libsnmp_handle,
		    "snmp_set_suffix_only");
		if (snmp_set_suffix_only_p != NULL) {
			/
			(*snmp_set_suffix_only_p)(2);
		} else {
			/
			ds_set_int_p = dlsym(libsnmp_handle, "ds_set_int");
			if (ds_set_int_p != NULL) {
				/
				(*ds_set_int_p)(0, 4, 2);
			}
		}
		dlclose(libsnmp_handle);
	}
#else /
	snmp_set_suffix_only(2);
#endif /
#endif /
#endif /
        proto_snmp = proto_register_protocol("Simple Network Management Protocol",
	    "SNMP", "snmp");
        proto_smux = proto_register_protocol("SNMP Multiplex Protocol",
	    "SMUX", "smux");
        proto_register_field_array(proto_snmp, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
	snmp_handle = create_dissector_handle(dissect_snmp, proto_snmp);
}
