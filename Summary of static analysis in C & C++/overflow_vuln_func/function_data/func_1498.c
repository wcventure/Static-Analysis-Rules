void
proto_register_gtp(void)
{
	static hf_register_info hf_gtp[] = {
		{ &hf_gtp_apn, { "APN", "gtp.apn", FT_STRING, BASE_DEC, NULL, 0, "Access Point Name", HFILL }},
		{ &hf_gtp_cause, { "Cause ", "gtp.cause", FT_UINT8, BASE_DEC, VALS(cause_type), 0, "Cause of operation", HFILL }},
		{ &hf_gtp_chrg_char, { "Charging characteristics", "gtp.chrg_char", FT_UINT16, BASE_DEC, NULL, 0, "Charging characteristics", HFILL }},
		{ &hf_gtp_chrg_char_s, { "Spare", "gtp.chrg_char_s", FT_UINT16, 	BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_S, "Spare", HFILL }},
		{ &hf_gtp_chrg_char_n, { "Normal charging", "gtp.chrg_char_n", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_N, "Normal charging", HFILL }},
		{ &hf_gtp_chrg_char_p, { "Prepaid charging", "gtp.chrg_char_p", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_P, "Prepaid charging", HFILL }},
		{ &hf_gtp_chrg_char_f, { "Flat rate charging", "gtp.chrg_char_f", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_F, "Flat rate charging", HFILL }},
		{ &hf_gtp_chrg_char_h, { "Hot billing charging", "gtp.chrg_char_h", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_H, "Hot billing charging", HFILL }},
		{ &hf_gtp_chrg_char_r, { "Reserved", "gtp.chrg_char_r", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_R, "Reserved", HFILL }},
		{ &hf_gtp_chrg_id, { "Charging ID", "gtp.chrg_id", FT_UINT32, BASE_HEX, NULL, 0, "Charging ID", HFILL }},
		{ &hf_gtp_chrg_ipv4, { "CG address IPv4", "gtp.chrg_ipv4", FT_IPv4, BASE_DEC, NULL, 0, "Charging Gateway address IPv4", HFILL }},
		{ &hf_gtp_chrg_ipv6, { "CG address IPv6", "gtp.chrg_ipv6", FT_IPv6, BASE_HEX, NULL, 0, "Charging Gateway address IPv6", HFILL }},
		{ &hf_gtp_ext_flow_label, { "Flow Label Data I", "gtp.ext_flow_label", FT_UINT16, BASE_HEX, NULL, 0, "Flow label data", HFILL }},
		{ &hf_gtp_ext_id, { "Extension identifier", "gtp.ext_id", FT_UINT16, BASE_DEC, NULL, 0, "Extension Identifier", HFILL }},
		{ &hf_gtp_ext_val, { "Extension value", "gtp.ext_val", FT_STRING, BASE_DEC, NULL, 0, "Extension Value", HFILL }},
		{ &hf_gtp_flags, { "Flags", "gtp.flags", FT_UINT8, BASE_HEX, NULL, 0, "Ver/PT/Spare...", HFILL }},
		{ &hf_gtp_flags_ver, 
			{ "Version", "gtp.flags.version", 
			FT_UINT8, BASE_DEC, VALS(ver_types), GTP_VER_MASK, 
			"GTP Version", HFILL }
		},
		{ &hf_gtp_flags_pt, 
			{ "Protocol type",	"gtp.flags.payload", 
			FT_UINT8, BASE_DEC, VALS(pt_types), GTP_PT_MASK,
			"Protocol Type", HFILL }
		},
		{ &hf_gtp_flags_spare1,
			{ "Reserved", "gtp.flags.reserved", 
			FT_UINT8, BASE_DEC, NULL, GTP_SPARE1_MASK, 
			"Reserved (shall be sent as '111' )", HFILL }
		},
		{ &hf_gtp_flags_snn, { "Is SNDCP N-PDU included?", "gtp.flags.snn", FT_BOOLEAN, 8, TFS(&yes_no_tfs), GTP_SNN_MASK, "Is SNDCP N-PDU LLC Number included? (1 = yes, 0 = no)", HFILL }},
		{ &hf_gtp_flags_spare2,	{ "Reserved", "gtp.flags.reserved", FT_UINT8, BASE_DEC, NULL, GTP_SPARE2_MASK, "Reserved (shall be sent as '1' )", HFILL }},
		{ &hf_gtp_flags_e, { "Is Next Extension Header present?", "gtp.flags.e", FT_BOOLEAN, 8, TFS(&yes_no_tfs), GTP_E_MASK, "Is Next Extension Header present? (1 = yes, 0 = no)", HFILL }},
		{ &hf_gtp_flags_s, { "Is Sequence Number present?", "gtp.flags.s", FT_BOOLEAN, 8, TFS(&yes_no_tfs), GTP_S_MASK, "Is Sequence Number present? (1 = yes, 0 = no)", HFILL }},
		{ &hf_gtp_flags_pn, { "Is N-PDU number present?", "gtp.flags.pn", FT_BOOLEAN, 8, TFS(&yes_no_tfs), GTP_PN_MASK, "Is N-PDU number present? (1 = yes, 0 = no)", HFILL }},
		{ &hf_gtp_flow_ii, { "Flow Label Data II ", "gtp.flow_ii", FT_UINT16, BASE_DEC, NULL, 0, "Downlink flow label data", HFILL }},
		{ &hf_gtp_flow_label, { "Flow label", "gtp.flow_label", FT_UINT16, BASE_HEX, NULL, 0, "Flow label", HFILL }},
		{ &hf_gtp_flow_sig, { "Flow label Signalling", "gtp.flow_sig", FT_UINT16, BASE_HEX, NULL, 0, "Flow label signalling", HFILL }},
		{ &hf_gtp_gsn_addr_len,	{ "GSN Address Length", "gtp.gsn_addr_len", FT_UINT8, BASE_DEC, NULL, GTP_EXT_GSN_ADDR_LEN_MASK, "GSN Address Length", HFILL }},
		{ &hf_gtp_gsn_addr_type, { "GSN Address Type", "gtp.gsn_addr_type", FT_UINT8, BASE_DEC, VALS(gsn_addr_type), GTP_EXT_GSN_ADDR_TYPE_MASK, "GSN Address Type", HFILL }},
		{ &hf_gtp_gsn_ipv4, { "GSN address IPv4", "gtp.gsn_ipv4", FT_IPv4, BASE_DEC, NULL, 0, "GSN address IPv4", HFILL }},
		{ &hf_gtp_gsn_ipv6, { "GSN address IPv6", "gtp.gsn_ipv6", FT_IPv6, BASE_DEC, NULL, 0, "GSN address IPv6", HFILL }},	
		{ &hf_gtp_imsi, { "IMSI", "gtp.imsi", FT_STRING, BASE_DEC, NULL, 0, "International Mobile Subscriber Identity number", HFILL }},
		{ &hf_gtp_length, { "Length", "gtp.length", FT_UINT16, BASE_DEC, NULL, 0, "Length (i.e. number of octets after TID or TEID)", HFILL }},
		{ &hf_gtp_map_cause, { "MAP cause", "gtp.map_cause", FT_UINT8, BASE_DEC, VALS(map_cause_type), 0, "MAP cause", HFILL }},
		{ &hf_gtp_message_type, { "Message Type", "gtp.message", FT_UINT8, BASE_HEX, VALS(message_type), 0x0, "GTP Message Type", HFILL }},
		{ &hf_gtp_ms_reason, { "MS not reachable reason", "gtp.ms_reason", FT_UINT8, BASE_DEC, VALS(ms_not_reachable_type), 0, "MS Not Reachable Reason", HFILL }},
		{ &hf_gtp_ms_valid, { "MS validated", "gtp.ms_valid", FT_BOOLEAN, BASE_NONE,NULL, 0, "MS validated", HFILL }},
		{ &hf_gtp_msisdn, { "MSISDN", "gtp.msisdn", FT_STRING, BASE_DEC, NULL, 0, "MS international PSTN/ISDN number", HFILL }},
		{ &hf_gtp_next, { "Next extension header type",	"gtp.next", FT_UINT8, BASE_HEX, NULL, 0, "Next Extension Header Type", HFILL }},
		{ &hf_gtp_node_ipv4, { "Node address IPv4", "gtp.node_ipv4", FT_IPv4, BASE_DEC, NULL, 0, "Recommended node address IPv4", HFILL }},
		{ &hf_gtp_node_ipv6, { "Node address IPv6", "gtp.node_ipv6", FT_IPv6, BASE_HEX, NULL, 0, "Recommended node address IPv6", HFILL }},
		{ &hf_gtp_npdu_number, { "N-PDU Number", "gtp.npdu_number", FT_UINT8, BASE_HEX, NULL, 0, "N-PDU Number", HFILL }},
		{ &hf_gtp_nsapi, { "NSAPI", "gtp.nsapi", FT_UINT8, BASE_DEC, NULL, 0, "Network layer Service Access Point Identifier", HFILL }},
		{ &hf_gtp_qos_version, { "Version", "gtp.qos_version", FT_STRING, BASE_DEC, NULL, 0, "Version of the QoS Profile", HFILL }},
		{ &hf_gtp_qos_spare1, { "Spare", "gtp.qos_spare1", FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE1_MASK, "Spare (shall be sent as '00' )", HFILL }},
		{ &hf_gtp_qos_delay, { "QoS delay", "gtp.qos_delay", FT_UINT8, BASE_DEC, VALS(qos_delay_type), GTP_EXT_QOS_DELAY_MASK, "Quality of Service Delay Class", HFILL }},
		{ &hf_gtp_qos_reliability, { "QoS reliability", "gtp.qos_reliabilty", FT_UINT8, BASE_DEC, VALS(qos_reliability_type), GTP_EXT_QOS_RELIABILITY_MASK, "Quality of Service Reliability Class", HFILL }},
		{ &hf_gtp_qos_peak, { "QoS peak", "gtp.qos_peak", FT_UINT8, BASE_DEC, VALS(qos_peak_type), GTP_EXT_QOS_PEAK_MASK, "Quality of Service Peak Throughput", HFILL }},
		{ &hf_gtp_qos_spare2, { "Spare", "gtp.qos_spare2",FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE2_MASK, "Spare (shall be sent as 0)", HFILL }},
		{ &hf_gtp_qos_precedence, { "QoS precedence", "gtp.qos_precedence", FT_UINT8, BASE_DEC, VALS(qos_precedence_type), GTP_EXT_QOS_PRECEDENCE_MASK, "Quality of Service Precedence Class", HFILL }},
		{ &hf_gtp_qos_spare3, { "Spare", "gtp.qos_spare3", FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE3_MASK, "Spare (shall be sent as '000' )", HFILL }},
		{ &hf_gtp_qos_mean, { "QoS mean", "gtp.qos_mean", FT_UINT8, BASE_DEC, VALS(qos_mean_type), GTP_EXT_QOS_MEAN_MASK, "Quality of Service Mean Throughput", HFILL }},
		{ &hf_gtp_qos_al_ret_priority, { "Allocation/Retention priority ","gtp.qos_al_ret_priority", FT_UINT8, BASE_DEC, NULL, 0, "Allocation/Retention Priority", HFILL }},
		{ &hf_gtp_qos_traf_class, { "Traffic class", "gtp.qos_traf_class", FT_UINT8, BASE_DEC, VALS(qos_traf_class), GTP_EXT_QOS_TRAF_CLASS_MASK, "Traffic Class", HFILL }},
		{ &hf_gtp_qos_del_order, { "Delivery order", "gtp.qos_del_order", FT_UINT8, BASE_DEC, VALS(qos_del_order), GTP_EXT_QOS_DEL_ORDER_MASK, "Delivery Order", HFILL }},
		{ &hf_gtp_qos_del_err_sdu, { "Delivery of erroneous SDU", "gtp.qos_del_err_sdu", FT_UINT8, BASE_DEC, VALS(qos_del_err_sdu), GTP_EXT_QOS_DEL_ERR_SDU_MASK, "Delivery of Erroneous SDU", HFILL }},
		{ &hf_gtp_qos_max_sdu_size, { "Maximum SDU size", "gtp.qos_max_sdu_size", FT_UINT8, BASE_DEC, VALS(qos_max_sdu_size), 0, "Maximum SDU size", HFILL }},
		{ &hf_gtp_qos_max_ul, { "Maximum bit rate for uplink",	"gtp.qos_max_ul", FT_UINT8, BASE_DEC, VALS(qos_max_ul), 0, "Maximum bit rate for uplink", HFILL }},
		{ &hf_gtp_qos_max_dl, { "Maximum bit rate for downlink", "gtp.qos_max_dl", FT_UINT8, BASE_DEC, VALS(qos_max_dl), 0, "Maximum bit rate for downlink", HFILL }},
		{ &hf_gtp_qos_res_ber, { "Residual BER", "gtp.qos_res_ber", FT_UINT8, BASE_DEC, VALS(qos_res_ber), GTP_EXT_QOS_RES_BER_MASK, "Residual Bit Error Rate", HFILL }},
		{ &hf_gtp_qos_sdu_err_ratio, { "SDU Error ratio", "gtp.qos_sdu_err_ratio", FT_UINT8, BASE_DEC, VALS(qos_sdu_err_ratio), GTP_EXT_QOS_SDU_ERR_RATIO_MASK, "SDU Error Ratio", HFILL }},
		{ &hf_gtp_qos_trans_delay, { "Transfer delay",	"gtp.qos_trans_delay", FT_UINT8, BASE_DEC, VALS(qos_trans_delay), GTP_EXT_QOS_TRANS_DELAY_MASK, "Transfer Delay", HFILL }},
		{ &hf_gtp_qos_traf_handl_prio, { "Traffic handling priority", "gtp.qos_traf_handl_prio", FT_UINT8, BASE_DEC, VALS(qos_traf_handl_prio), GTP_EXT_QOS_TRAF_HANDL_PRIORITY_MASK, "Traffic Handling Priority", HFILL }},
		{ &hf_gtp_qos_guar_ul, { "Guaranteed bit rate for uplink", "gtp.qos_guar_ul", FT_UINT8,	BASE_DEC, VALS(qos_guar_ul), 0, "Guaranteed bit rate for uplink", HFILL }},
		{ &hf_gtp_qos_guar_dl, { "Guaranteed bit rate for downlink", "gtp.qos_guar_dl",	FT_UINT8, BASE_DEC, VALS(qos_guar_dl), 0, "Guaranteed bit rate for downlink", HFILL }},
		{ &hf_gtp_pkt_flow_id, { "Packet Flow ID", "gtp.pkt_flow_id", FT_UINT8, BASE_DEC, NULL, 0, "Packet Flow ID", HFILL }},
		{ &hf_gtp_ptmsi, { "P-TMSI", "gtp.ptmsi", FT_UINT32, BASE_HEX, NULL, 0, "Packet-Temporary Mobile Subscriber Identity", HFILL }},
		{ &hf_gtp_ptmsi_sig, { "P-TMSI Signature", "gtp.ptmsi_sig", FT_UINT24, BASE_HEX, NULL, 0, "P-TMSI Signature", HFILL }},
		{ &hf_gtp_rab_gtpu_dn, { "Downlink GTP-U seq number", "gtp.rab_gtp_dn", FT_UINT16, BASE_DEC, NULL, 0, "Downlink GTP-U sequence number", HFILL }},
		{ &hf_gtp_rab_gtpu_up, { "Uplink GTP-U seq number", "gtp.rab_gtp_up", FT_UINT16, BASE_DEC, NULL, 0, "Uplink GTP-U sequence number", HFILL }},
		{ &hf_gtp_rab_pdu_dn, { "Downlink next PDCP-PDU seq number", "gtp.rab_pdu_dn", FT_UINT8, BASE_DEC, NULL, 0, "Downlink next PDCP-PDU sequence number", HFILL }},
		{ &hf_gtp_rab_pdu_up, { "Uplink next PDCP-PDU seq number", "gtp.rab_pdu_up", FT_UINT8, BASE_DEC, NULL, 0, "Uplink next PDCP-PDU sequence number", HFILL }},
		{ &hf_gtp_rai_mcc, { "MCC", "gtp.mcc", FT_UINT16, BASE_DEC, NULL, 0, "Mobile Country Code", HFILL }},
		{ &hf_gtp_rai_mnc, { "MNC", "gtp.mnc", FT_UINT8, BASE_DEC, NULL, 0, "Mobile Network Code", HFILL }},
		{ &hf_gtp_rai_rac, { "RAC", "gtp.rac", FT_UINT8, BASE_DEC, NULL, 0, "Routing Area Code", HFILL }},
		{ &hf_gtp_rai_lac, { "LAC", "gtp.lac", FT_UINT16, BASE_DEC, NULL, 0, "Location Area Code", HFILL }},
		{ &hf_gtp_ranap_cause, { "RANAP cause", "gtp.ranap_cause", FT_UINT8, BASE_DEC, VALS(ranap_cause_type), 0, "RANAP cause", HFILL }},
		{ &hf_gtp_recovery, { "Recovery", "gtp.recovery", FT_UINT8, BASE_DEC, NULL, 0, "Restart counter", HFILL }},
		{ &hf_gtp_reorder, { "Reordering required","gtp.reorder", FT_BOOLEAN, BASE_NONE,NULL, 0, "Reordering required", HFILL }},
		{ &hf_gtp_rnc_ipv4, { "RNC address IPv4", "gtp.rnc_ipv4", FT_IPv4, BASE_DEC, NULL, 0, "Radio Network Controller address IPv4", HFILL }},
		{ &hf_gtp_rnc_ipv6, { "RNC address IPv6", "gtp.rnc_ipv6", FT_IPv6, BASE_HEX, NULL, 0, "Radio Network Controller address IPv6", HFILL }},
		{ &hf_gtp_rp, { "Radio Priority", "gtp.rp", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_MASK, "Radio Priority for uplink tx", HFILL }},
		{ &hf_gtp_rp_nsapi, { "NSAPI in Radio Priority", "gtp.rp_nsapi", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_NSAPI_MASK, "Network layer Service Access Point Identifier in Radio Priority", HFILL }},
		{ &hf_gtp_rp_sms, { "Radio Priority SMS", "gtp.rp_sms",	FT_UINT8, BASE_DEC, NULL, 0, "Radio Priority for MO SMS", HFILL }},
		{ &hf_gtp_rp_spare, { "Reserved", "gtp.rp_spare", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_SPARE_MASK, "Spare bit", HFILL }},		
		{ &hf_gtp_sel_mode, { "Selection mode", "gtp.sel_mode", FT_UINT8, BASE_DEC, VALS(sel_mode_type), 0, "Selection Mode", HFILL }},
		{ &hf_gtp_seq_number, { "Sequence number", "gtp.seq_number", FT_UINT16, BASE_HEX, NULL, 0, "Sequence Number", HFILL }},
		{ &hf_gtp_sndcp_number, { "SNDCP N-PDU LLC Number", "gtp.sndcp_number", FT_UINT8, BASE_HEX, NULL, 0, "SNDCP N-PDU LLC Number", HFILL }},
		{ &hf_gtp_tear_ind, { "Teardown Indicator", "gtp.tear_ind", FT_BOOLEAN, BASE_NONE,NULL, 0, "Teardown Indicator", HFILL }},
		{ &hf_gtp_teid, { "TEID", "gtp.teid", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier", HFILL }},
		{ &hf_gtp_teid_cp, { "TEID Control Plane", "gtp.teid_cp", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Control Plane", HFILL }},
		{ &hf_gtp_teid_data, { "TEID Data I", "gtp.teid_data", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Data I", HFILL }},
		{ &hf_gtp_teid_ii, { "TEID Data II", "gtp.teid_ii", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Data II", HFILL }},
		{ &hf_gtp_tft_code, { "TFT operation code", "gtp.tft_code", FT_UINT8, BASE_DEC, VALS (tft_code_type), GTPv1_TFT_CODE_MASK, "TFT operation code", HFILL }},
		{ &hf_gtp_tft_spare, { "TFT spare bit",	"gtp.tft_spare", FT_UINT8, BASE_DEC, NULL, GTPv1_TFT_SPARE_MASK, "TFT spare bit", HFILL }},
		{ &hf_gtp_tft_number, { "Number of packet filters", "gtp.tft_number", FT_UINT8, BASE_DEC, NULL, GTPv1_TFT_NUMBER_MASK, "Number of packet filters", HFILL }},
		{ &hf_gtp_tft_eval, { "Evaluation precedence", "gtp.tft_eval", FT_UINT8, BASE_DEC, NULL, 0, "Evaluation precedence", HFILL }},
		{ &hf_gtp_tid, { "TID", "gtp.tid", FT_STRING, BASE_DEC, NULL, 0, "Tunnel Identifier", HFILL }},
		{ &hf_gtp_tlli, { "TLLI", "gtp.tlli", FT_UINT32, BASE_HEX, NULL, 0, "Temporary Logical Link Identity", HFILL }},
		{ &hf_gtp_tr_comm, { "Packet transfer command",	"gtp.tr_comm", FT_UINT8, BASE_DEC, VALS (tr_comm_type), 0, "Packat transfer command", HFILL }},
		{ &hf_gtp_trace_ref, { "Trace reference", "gtp.trace_ref", FT_UINT16, BASE_HEX, NULL, 0, "Trace reference", HFILL }},
		{ &hf_gtp_trace_type, { "Trace type", "gtp.trace_type", FT_UINT16, BASE_HEX, NULL, 0, "Trace type", HFILL }},
		{ &hf_gtp_unknown, { "Unknown data (length)",	"gtp.unknown", FT_UINT16, BASE_DEC, NULL, 0, "Unknown data", HFILL }},
		{ &hf_gtp_user_addr_pdp_org, { "PDP type organization", "gtp.user_addr_pdp_org", FT_UINT8, BASE_DEC, VALS(pdp_org_type), 0, "PDP type organization", HFILL }},
		{ &hf_gtp_user_addr_pdp_type, { "PDP type number", "gtp.user_addr_pdp_type", FT_UINT8, BASE_HEX, VALS (pdp_type), 0, "PDP type", HFILL }},
		{ &hf_gtp_user_ipv4, { "End user address IPv4", "gtp.user_ipv4", FT_IPv4, BASE_DEC, NULL, 0, "End user address IPv4", HFILL }},
		{ &hf_gtp_user_ipv6, { "End user address IPv6", "gtp.user_ipv6", FT_IPv6, BASE_HEX, NULL, 0, "End user address IPv6", HFILL }},		
		{ &hf_gtp_security_mode, 
			{ "Security Mode", "gtp.security_mode", 
			FT_UINT8, BASE_DEC, VALS(mm_sec_modep), 0xc0, 
			"Security Mode", HFILL }
		},
		{ &hf_gtp_no_of_vectors,
			{ "No of Vectors", "gtp.no_of_vectors", 
			FT_UINT8, BASE_DEC, NULL, 0x38, 
			"No of Vectors", HFILL }
		},
		{ &hf_gtp_cipher_algorithm,
			{ "Cipher Algorithm", "gtp.no_of_vectors", 
			FT_UINT8, BASE_DEC, VALS(gtp_cipher_algorithm), 0x07, 
			"Cipher Algorithm", HFILL }
		},
		{ &hf_gtp_cksn_ksi,
			{ "Ciphering Key Sequence Number (CKSN)/Key Set Identifier (KSI)", "gtp.cksn_ksi", 
			FT_UINT8, BASE_DEC, NULL, 0x07, 
			"CKSN/KSI", HFILL }
		},
		{ &hf_gtp_cksn,
			{ "Ciphering Key Sequence Number (CKSN)", "gtp.cksn_ksi", 
			FT_UINT8, BASE_DEC, NULL, 0x07, 
			"CKSN", HFILL }
		},
		{ &hf_gtp_ksi,
			{ "Key Set Identifier (KSI)", "gtp.cksn_ksi", 
			FT_UINT8, BASE_DEC, NULL, 0x07, 
			"KSI", HFILL }
		},
	};
	
	static gint *ett_gtp_array[] = {
		&ett_gtp,
		&ett_gtp_flags,
		&ett_gtp_ext,
		&ett_gtp_rai,
		&ett_gtp_qos,
		&ett_gtp_auth_tri,
		&ett_gtp_flow_ii,
		&ett_gtp_rab_cntxt,
		&ett_gtp_rp,
		&ett_gtp_pkt_flow_id,
		&ett_gtp_chrg_char,
		&ett_gtp_user,
		&ett_gtp_mm,
		&ett_gtp_trip,
		&ett_gtp_quint,
		&ett_gtp_pdp,
		&ett_gtp_apn,
		&ett_gtp_proto,
		&ett_gtp_gsn_addr,
		&ett_gtp_tft,
		&ett_gtp_tft_pf,
		&ett_gtp_tft_flags,
		&ett_gtp_rab_setup,
		&ett_gtp_hdr_list,
		&ett_gtp_chrg_addr,
		&ett_gtp_node_addr,
		&ett_gtp_rel_pack,
		&ett_gtp_can_pack,
		&ett_gtp_data_resp,
		&ett_gtp_priv_ext,
		&ett_gtp_net_cap,
	};

	module_t	*gtp_module;

	proto_gtp = proto_register_protocol ("GPRS Tunneling Protocol", "GTP", "gtp");
	proto_register_field_array (proto_gtp, hf_gtp, array_length (hf_gtp));
	proto_register_subtree_array (ett_gtp_array, array_length (ett_gtp_array));
	
	gtp_module = prefs_register_protocol(proto_gtp, proto_reg_handoff_gtp);

	prefs_register_uint_preference(gtp_module, "v0_port", "GTPv0 port", "GTPv0 port (default 3386)", 10, &g_gtpv0_port);
	prefs_register_uint_preference(gtp_module, "v1c_port", "GTPv1 control plane (GTP-C) port", "GTPv1 control plane port (default 2123)", 10, &g_gtpv1c_port);
	prefs_register_uint_preference(gtp_module, "v1u_port", "GTPv1 user plane (GTP-U) port", "GTPv1 user plane port (default 2152)", 10, &g_gtpv1u_port);
	prefs_register_bool_preference(gtp_module, "dissect_tpdu", "Dissect T-PDU", "Dissect T-PDU", &gtp_tpdu);
	
	prefs_register_obsolete_preference (gtp_module, "v0_dissect_cdr_as");
	prefs_register_obsolete_preference (gtp_module, "v0_check_etsi");
	prefs_register_obsolete_preference (gtp_module, "v1_check_etsi");
	prefs_register_bool_preference (gtp_module, "check_etsi", "Compare GTP order with ETSI", "GTP ETSI order", &gtp_etsi_order);
	prefs_register_obsolete_preference(gtp_module, "ppp_reorder");
	
	/
	prefs_register_bool_preference(gtp_module, "dissect_gtp_over_tcp", "Dissect GTP over TCP", "Dissect GTP over TCP", &gtp_over_tcp);

	register_dissector("gtp", dissect_gtp, proto_gtp);
}
