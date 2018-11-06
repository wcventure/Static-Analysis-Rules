void
proto_reg_handoff_gtp(void)
{
	static int Initialized = FALSE;
	static dissector_handle_t gtp_handle;

	
	if (!Initialized) {
		gtp_handle = find_dissector("gtp");
		ppp_subdissector_table = find_dissector_table("ppp.protocol");
		
		radius_register_avp_dissector(VENDOR_THE3GPP,5,dissect_radius_qos_umts);

		Initialized = TRUE;
	} else {
		dissector_delete ("udp.port", gtpv0_port, gtp_handle);
		dissector_delete ("udp.port", gtpv1c_port, gtp_handle);
		dissector_delete ("udp.port", gtpv1u_port, gtp_handle);
		
		if ( !gtp_over_tcp ) {
			dissector_delete ("tcp.port", gtpv0_port, gtp_handle);
			dissector_delete ("tcp.port", gtpv1c_port, gtp_handle);
			dissector_delete ("tcp.port", gtpv1u_port, gtp_handle);
		}
		
	}

	gtpv0_port = g_gtpv0_port;
	gtpv1c_port = g_gtpv1c_port;
	gtpv1u_port = g_gtpv1u_port;

	dissector_add ("udp.port", g_gtpv0_port, gtp_handle);
	dissector_add ("udp.port", g_gtpv1c_port, gtp_handle);
	dissector_add ("udp.port", g_gtpv1u_port, gtp_handle);

	
	if ( gtp_over_tcp ) {
		dissector_add ("tcp.port", g_gtpv0_port, gtp_handle);
		dissector_add ("tcp.port", g_gtpv1c_port, gtp_handle);
		dissector_add ("tcp.port", g_gtpv1u_port, gtp_handle);
	}
	
	ip_handle = find_dissector("ip");
	ipv6_handle = find_dissector("ipv6");
	ppp_handle = find_dissector("ppp");
	data_handle = find_dissector("data");
	gtpcdr_handle = find_dissector("gtpcdr");
	bssap_pdu_type_table = find_dissector_table("bssap.pdu_type");

}
