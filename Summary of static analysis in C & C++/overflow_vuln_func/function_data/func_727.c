void proto_register_gtp(void)
{
    module_t *gtp_module;
    guint    i;
    guint    last_offset;

    static hf_register_info hf_gtp[] = {

        {&hf_gtp_ie_id,
         {"IE Id", "gtp.ie_id", 
		 FT_UINT8, BASE_DEC|BASE_EXT_STRING, &gtp_val_ext, 0x0, 
		 NULL, HFILL}
		},
        {&hf_gtp_ie_len,
         {"Length", "gtp.ie_len", 
		 FT_UINT16, BASE_DEC, NULL, 0x0, 
		 NULL, HFILL}
		},
        {&hf_gtp_response_in,
         {"Response In", "gtp.response_in", FT_FRAMENUM, BASE_NONE, NULL, 0x0, "The response to this GTP request is in this frame", HFILL}},
        {&hf_gtp_response_to,
         {"Response To", "gtp.response_to", FT_FRAMENUM, BASE_NONE, NULL, 0x0, "This is a response to the GTP request in this frame", HFILL}},
        {&hf_gtp_time, {"Time", "gtp.time", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0, "The time between the Request and the Response", HFILL}},
        {&hf_gtp_apn, {"APN", "gtp.apn", FT_STRING, BASE_NONE, NULL, 0, "Access Point Name", HFILL}},
        {&hf_gtp_cause, {"Cause", "gtp.cause", FT_UINT8, BASE_DEC|BASE_EXT_STRING, &cause_type_ext, 0, "Cause of operation", HFILL}},
        {&hf_gtp_chrg_char, {"Charging characteristics", "gtp.chrg_char", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_chrg_char_s, {"Spare", "gtp.chrg_char_s", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_S, NULL, HFILL}},
        {&hf_gtp_chrg_char_n, {"Normal charging", "gtp.chrg_char_n", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_N, NULL, HFILL}},
        {&hf_gtp_chrg_char_p, {"Prepaid charging", "gtp.chrg_char_p", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_P, NULL, HFILL}},
        {&hf_gtp_chrg_char_f,
         {"Flat rate charging", "gtp.chrg_char_f", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_F, NULL, HFILL}},
        {&hf_gtp_chrg_char_h,
         {"Hot billing charging", "gtp.chrg_char_h", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_H, NULL, HFILL}},
        {&hf_gtp_chrg_char_r, {"Reserved", "gtp.chrg_char_r", FT_UINT16, BASE_DEC, NULL, GTP_MASK_CHRG_CHAR_R, NULL, HFILL}},
        {&hf_gtp_chrg_id, {"Charging ID", "gtp.chrg_id", FT_UINT32, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_chrg_ipv4, {"CG address IPv4", "gtp.chrg_ipv4", FT_IPv4, BASE_NONE, NULL, 0, "Charging Gateway address IPv4", HFILL}},
        {&hf_gtp_chrg_ipv6, {"CG address IPv6", "gtp.chrg_ipv6", FT_IPv6, BASE_NONE, NULL, 0, "Charging Gateway address IPv6", HFILL}},
        {&hf_gtp_ext_flow_label, {"Flow Label Data I", "gtp.ext_flow_label", FT_UINT16, BASE_HEX, NULL, 0, "Flow label data", HFILL}},
        {&hf_gtp_ext_id, {"Extension identifier", "gtp.ext_id", FT_UINT16, BASE_DEC|BASE_EXT_STRING, &sminmpec_values_ext, 0, NULL, HFILL}},
        {&hf_gtp_ext_val, {"Extension value", "gtp.ext_val", FT_BYTES, BASE_NONE, NULL, 0, NULL, HFILL}},
        {&hf_gtp_flags, {"Flags", "gtp.flags", FT_UINT8, BASE_HEX, NULL, 0, "Ver/PT/Spare...", HFILL}},
        {&hf_gtp_ext_hdr, {"Extension header", "gtp.ext_hdr", FT_UINT8, BASE_HEX, VALS(next_extension_header_fieldvals), 0, NULL, HFILL}},
        {&hf_gtp_ext_hdr_next, {"Next extension header", "gtp.ext_hdr.next", FT_UINT8, BASE_HEX, VALS(next_extension_header_fieldvals), 0, NULL, HFILL}},
        {&hf_gtp_ext_hdr_pdcpsn, {"PDCP Sequence Number", "gtp.ext_hdr.pdcp_sn", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_ext_hdr_length, {"Extension Header Length", "gtp.ext_hdr.length", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_flags_ver,
         {"Version", "gtp.flags.version",
          FT_UINT8, BASE_DEC, VALS(ver_types), GTP_VER_MASK,
          "GTP Version", HFILL}
        },
        {&hf_gtp_prime_flags_ver,
         {"Version", "gtp.prim.flags.version",
          FT_UINT8, BASE_DEC,NULL, GTP_VER_MASK,
          "GTP' Version", HFILL}
        },
        {&hf_gtp_flags_pt,
         {"Protocol type", "gtp.flags.payload",
          FT_UINT8, BASE_DEC, VALS(pt_types), GTP_PT_MASK,
          NULL, HFILL}
        },
        {&hf_gtp_flags_spare1,
         {"Reserved", "gtp.flags.reserved",
          FT_UINT8, BASE_DEC, NULL, GTP_SPARE1_MASK,
          "Reserved (shall be sent as '111' )", HFILL}
        },
        {&hf_gtp_flags_hdr_length,
         {"Header length", "gtp.flags._hdr_length",
          FT_BOOLEAN, 8,  TFS(&gtp_hdr_length_vals), 0x01,
          NULL, HFILL}},
        {&hf_gtp_flags_snn,
         {"Is SNDCP N-PDU included?", "gtp.flags.snn", FT_BOOLEAN, 8, TFS(&tfs_yes_no), GTP_SNN_MASK,
          "Is SNDCP N-PDU LLC Number included? (1 = yes, 0 = no)", HFILL}},
        {&hf_gtp_flags_spare2,
         {"Reserved", "gtp.flags.reserved", FT_UINT8, BASE_DEC, NULL, GTP_SPARE2_MASK, "Reserved (shall be sent as '1' )", HFILL}},
        {&hf_gtp_flags_e,
         {"Is Next Extension Header present?", "gtp.flags.e", FT_BOOLEAN, 8, TFS(&tfs_yes_no), GTP_E_MASK,
          "Is Next Extension Header present? (1 = yes, 0 = no)", HFILL}},
        {&hf_gtp_flags_s,
         {"Is Sequence Number present?", "gtp.flags.s", FT_BOOLEAN, 8, TFS(&tfs_yes_no), GTP_S_MASK, "Is Sequence Number present? (1 = yes, 0 = no)",
          HFILL}},
        {&hf_gtp_flags_pn,
         {"Is N-PDU number present?", "gtp.flags.pn", FT_BOOLEAN, 8, TFS(&tfs_yes_no), GTP_PN_MASK, "Is N-PDU number present? (1 = yes, 0 = no)",
          HFILL}},
        {&hf_gtp_flow_ii, {"Flow Label Data II", "gtp.flow_ii", FT_UINT16, BASE_DEC, NULL, 0, "Downlink flow label data", HFILL}},
        {&hf_gtp_flow_label, {"Flow label", "gtp.flow_label", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_flow_sig, {"Flow label Signalling", "gtp.flow_sig", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_gsn_addr_len,
         {"GSN Address Length", "gtp.gsn_addr_len", FT_UINT8, BASE_DEC, NULL, GTP_EXT_GSN_ADDR_LEN_MASK, NULL, HFILL}},
        {&hf_gtp_gsn_addr_type,
         {"GSN Address Type", "gtp.gsn_addr_type", FT_UINT8, BASE_DEC, VALS(gsn_addr_type), GTP_EXT_GSN_ADDR_TYPE_MASK, NULL, HFILL}},
        {&hf_gtp_gsn_ipv4, {"GSN address IPv4", "gtp.gsn_ipv4", FT_IPv4, BASE_NONE, NULL, 0, NULL, HFILL}},
        {&hf_gtp_gsn_ipv6, {"GSN address IPv6", "gtp.gsn_ipv6", FT_IPv6, BASE_NONE, NULL, 0, NULL, HFILL}},
        {&hf_gtp_imsi, {"IMSI", "gtp.imsi", FT_STRING, BASE_NONE, NULL, 0, "International Mobile Subscriber Identity number", HFILL}},
        {&hf_gtp_length, {"Length", "gtp.length", FT_UINT16, BASE_DEC, NULL, 0, "Length (i.e. number of octets after TID or TEID)", HFILL}},
        {&hf_gtp_map_cause, {"MAP cause", "gtp.map_cause", FT_UINT8, BASE_DEC, VALS(gsm_old_GSMMAPLocalErrorcode_vals), 0, NULL, HFILL}},
        {&hf_gtp_message_type, {"Message Type", "gtp.message", FT_UINT8, BASE_HEX|BASE_EXT_STRING, &gtp_message_type_ext, 0x0, "GTP Message Type", HFILL}},
        {&hf_gtp_ms_reason,
         {"MS not reachable reason", "gtp.ms_reason", FT_UINT8, BASE_DEC, VALS(ms_not_reachable_type), 0, NULL, HFILL}},
        {&hf_gtp_ms_valid, {"MS validated", "gtp.ms_valid", FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL}},
        {&hf_gtp_next,
         {"Next extension header type", "gtp.next", FT_UINT8, BASE_HEX, VALS(next_extension_header_fieldvals), 0, NULL,
          HFILL}},
        {&hf_gtp_node_ipv4, {"Node address IPv4", "gtp.node_ipv4", FT_IPv4, BASE_NONE, NULL, 0, "Recommended node address IPv4", HFILL}},
        {&hf_gtp_node_ipv6, {"Node address IPv6", "gtp.node_ipv6", FT_IPv6, BASE_NONE, NULL, 0, "Recommended node address IPv6", HFILL}},
        {&hf_gtp_npdu_number, {"N-PDU Number", "gtp.npdu_number", FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_nsapi, {"NSAPI", "gtp.nsapi", FT_UINT8, BASE_DEC, NULL, 0, "Network layer Service Access Point Identifier", HFILL}},
        {&hf_gtp_qos_version, {"Version", "gtp.qos_version", FT_STRING, BASE_NONE, NULL, 0, "Version of the QoS Profile", HFILL}},
        {&hf_gtp_qos_spare1, {"Spare", "gtp.qos_spare1", FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE1_MASK, "Spare (shall be sent as '00' )", HFILL}},
        {&hf_gtp_qos_delay,
         {"QoS delay", "gtp.qos_delay", FT_UINT8, BASE_DEC, VALS(qos_delay_type), GTP_EXT_QOS_DELAY_MASK, "Quality of Service Delay Class", HFILL}},
        {&hf_gtp_qos_reliability,
         {"QoS reliability", "gtp.qos_reliability", FT_UINT8, BASE_DEC, VALS(qos_reliability_type), GTP_EXT_QOS_RELIABILITY_MASK,
          "Quality of Service Reliability Class", HFILL}},
        {&hf_gtp_qos_peak,
         {"QoS peak", "gtp.qos_peak", FT_UINT8, BASE_DEC, VALS(qos_peak_type), GTP_EXT_QOS_PEAK_MASK, "Quality of Service Peak Throughput", HFILL}},
        {&hf_gtp_qos_spare2, {"Spare", "gtp.qos_spare2", FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE2_MASK, "Spare (shall be sent as 0)", HFILL}},
        {&hf_gtp_qos_precedence,
         {"QoS precedence", "gtp.qos_precedence", FT_UINT8, BASE_DEC, VALS(qos_precedence_type), GTP_EXT_QOS_PRECEDENCE_MASK,
          "Quality of Service Precedence Class", HFILL}},
        {&hf_gtp_qos_spare3,
         {"Spare", "gtp.qos_spare3", FT_UINT8, BASE_DEC, NULL, GTP_EXT_QOS_SPARE3_MASK, "Spare (shall be sent as '000' )", HFILL}},
        {&hf_gtp_qos_mean,
         {"QoS mean", "gtp.qos_mean", FT_UINT8, BASE_DEC|BASE_EXT_STRING, &qos_mean_type_ext, GTP_EXT_QOS_MEAN_MASK, "Quality of Service Mean Throughput", HFILL}},
        {&hf_gtp_qos_al_ret_priority,
         {"Allocation/Retention priority", "gtp.qos_al_ret_priority", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_qos_traf_class,
         {"Traffic class", "gtp.qos_traf_class", FT_UINT8, BASE_DEC, VALS(qos_traf_class), GTP_EXT_QOS_TRAF_CLASS_MASK, NULL, HFILL}},
        {&hf_gtp_qos_del_order,
         {"Delivery order", "gtp.qos_del_order", FT_UINT8, BASE_DEC, VALS(qos_del_order), GTP_EXT_QOS_DEL_ORDER_MASK, NULL, HFILL}},
        {&hf_gtp_qos_del_err_sdu,
         {"Delivery of erroneous SDU", "gtp.qos_del_err_sdu", FT_UINT8, BASE_DEC, VALS(qos_del_err_sdu), GTP_EXT_QOS_DEL_ERR_SDU_MASK,
          NULL, HFILL}},
        {&hf_gtp_qos_max_sdu_size,
         {"Maximum SDU size", "gtp.qos_max_sdu_size", FT_UINT8, BASE_DEC, VALS(qos_max_sdu_size), 0, NULL, HFILL}},
        {&hf_gtp_qos_max_ul,
         {"Maximum bit rate for uplink", "gtp.qos_max_ul", FT_UINT8, BASE_DEC, VALS(qos_max_ul), 0, NULL, HFILL}},
        {&hf_gtp_qos_max_dl,
         {"Maximum bit rate for downlink", "gtp.qos_max_dl", FT_UINT8, BASE_DEC, VALS(qos_max_dl), 0, NULL, HFILL}},
        {&hf_gtp_qos_res_ber,
         {"Residual BER", "gtp.qos_res_ber", FT_UINT8, BASE_DEC, VALS(qos_res_ber), GTP_EXT_QOS_RES_BER_MASK, "Residual Bit Error Rate", HFILL}},
        {&hf_gtp_qos_sdu_err_ratio,
         {"SDU Error ratio", "gtp.qos_sdu_err_ratio", FT_UINT8, BASE_DEC, VALS(qos_sdu_err_ratio), GTP_EXT_QOS_SDU_ERR_RATIO_MASK, NULL,
          HFILL}},
        {&hf_gtp_qos_trans_delay,
         {"Transfer delay", "gtp.qos_trans_delay", FT_UINT8, BASE_DEC|BASE_EXT_STRING, &qos_trans_delay_ext, GTP_EXT_QOS_TRANS_DELAY_MASK, NULL, HFILL}},
        {&hf_gtp_qos_traf_handl_prio,
         {"Traffic handling priority", "gtp.qos_traf_handl_prio", FT_UINT8, BASE_DEC, VALS(qos_traf_handl_prio), GTP_EXT_QOS_TRAF_HANDL_PRIORITY_MASK,
          NULL, HFILL}},
        {&hf_gtp_qos_guar_ul,
         {"Guaranteed bit rate for uplink", "gtp.qos_guar_ul", FT_UINT8, BASE_DEC, VALS(qos_guar_ul), 0, NULL, HFILL}},
        {&hf_gtp_qos_guar_dl,
         {"Guaranteed bit rate for downlink", "gtp.qos_guar_dl", FT_UINT8, BASE_DEC, VALS(qos_guar_dl), 0, NULL,
          HFILL}},
        {&hf_gtp_qos_src_stat_desc,
         {"Source Statistics Descriptor", "gtp.src_stat_desc", FT_UINT8, BASE_DEC, VALS(src_stat_desc_vals), GTP_EXT_QOS_SRC_STAT_DESC_MASK, NULL, HFILL}},
        {&hf_gtp_qos_sig_ind,
         {"Signalling Indication", "gtp.sig_ind", FT_BOOLEAN, 8, TFS(&gtp_sig_ind), GTP_EXT_QOS_SIG_IND_MASK, NULL, HFILL}},
        {&hf_gtp_pkt_flow_id, {"Packet Flow ID", "gtp.pkt_flow_id", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_ptmsi, {"P-TMSI", "gtp.ptmsi", FT_UINT32, BASE_HEX, NULL, 0, "Packet-Temporary Mobile Subscriber Identity", HFILL}},
        {&hf_gtp_ptmsi_sig, {"P-TMSI Signature", "gtp.ptmsi_sig", FT_UINT24, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_rab_gtpu_dn, {"Downlink GTP-U seq number", "gtp.rab_gtp_dn", FT_UINT16, BASE_DEC, NULL, 0, "Downlink GTP-U sequence number", HFILL}},
        {&hf_gtp_rab_gtpu_up, {"Uplink GTP-U seq number", "gtp.rab_gtp_up", FT_UINT16, BASE_DEC, NULL, 0, "Uplink GTP-U sequence number", HFILL}},
        {&hf_gtp_rab_pdu_dn,
         {"Downlink next PDCP-PDU seq number", "gtp.rab_pdu_dn", FT_UINT16, BASE_DEC, NULL, 0, "Downlink next PDCP-PDU sequence number", HFILL}},
        {&hf_gtp_rab_pdu_up,
         {"Uplink next PDCP-PDU seq number", "gtp.rab_pdu_up", FT_UINT16, BASE_DEC, NULL, 0, "Uplink next PDCP-PDU sequence number", HFILL}},
        {&hf_gtp_rai_rac, {"RAC", "gtp.rac", FT_UINT8, BASE_DEC, NULL, 0, "Routing Area Code", HFILL}},
        {&hf_gtp_rai_lac, {"LAC", "gtp.lac", FT_UINT16, BASE_DEC, NULL, 0, "Location Area Code", HFILL}},
        {&hf_gtp_ranap_cause, {"RANAP cause", "gtp.ranap_cause", FT_UINT8, BASE_DEC|BASE_EXT_STRING, &ranap_cause_type_ext, 0, NULL, HFILL}},
        {&hf_gtp_recovery, {"Recovery", "gtp.recovery", FT_UINT8, BASE_DEC, NULL, 0, "Restart counter", HFILL}},
        {&hf_gtp_reorder, {"Reordering required", "gtp.reorder", FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL}},
        {&hf_gtp_rnc_ipv4, {"RNC address IPv4", "gtp.rnc_ipv4", FT_IPv4, BASE_NONE, NULL, 0, "Radio Network Controller address IPv4", HFILL}},
        {&hf_gtp_rnc_ipv6, {"RNC address IPv6", "gtp.rnc_ipv6", FT_IPv6, BASE_NONE, NULL, 0, "Radio Network Controller address IPv6", HFILL}},
        {&hf_gtp_rp, {"Radio Priority", "gtp.rp", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_MASK, "Radio Priority for uplink tx", HFILL}},
        {&hf_gtp_rp_nsapi,
         {"NSAPI in Radio Priority", "gtp.rp_nsapi", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_NSAPI_MASK,
          "Network layer Service Access Point Identifier in Radio Priority", HFILL}},
        {&hf_gtp_rp_sms, {"Radio Priority SMS", "gtp.rp_sms", FT_UINT8, BASE_DEC, NULL, 0, "Radio Priority for MO SMS", HFILL}},
        {&hf_gtp_rp_spare, {"Reserved", "gtp.rp_spare", FT_UINT8, BASE_DEC, NULL, GTPv1_EXT_RP_SPARE_MASK, "Spare bit", HFILL}},
        {&hf_gtp_sel_mode,
         {"Selection mode", "gtp.sel_mode",
          FT_UINT8, BASE_DEC, VALS(sel_mode_type), 0x03,
          NULL, HFILL}
        },
        {&hf_gtp_seq_number, {"Sequence number", "gtp.seq_number", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_sndcp_number, {"SNDCP N-PDU LLC Number", "gtp.sndcp_number", FT_UINT8, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_tear_ind, {"Teardown Indicator", "gtp.tear_ind", FT_BOOLEAN, BASE_NONE, NULL, 0x0, NULL, HFILL}},
        {&hf_gtp_teid, {"TEID", "gtp.teid", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier", HFILL}},
        {&hf_gtp_teid_cp, {"TEID Control Plane", "gtp.teid_cp", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Control Plane", HFILL}},
        {&hf_gtp_ulink_teid_cp,
         {"Uplink TEID Control Plane", "gtp.ulink_teid_cp", FT_UINT32, BASE_HEX, NULL, 0, "Uplink Tunnel Endpoint Identifier Control Plane", HFILL}},
        {&hf_gtp_teid_data, {"TEID Data I", "gtp.teid_data", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Data I", HFILL}},
        {&hf_gtp_ulink_teid_data,
         {"Uplink TEID Data I", "gtp.ulink_teid_data", FT_UINT32, BASE_HEX, NULL, 0, "UplinkTunnel Endpoint Identifier Data I", HFILL}},
        {&hf_gtp_teid_ii, {"TEID Data II", "gtp.teid_ii", FT_UINT32, BASE_HEX, NULL, 0, "Tunnel Endpoint Identifier Data II", HFILL}},
        {&hf_gtp_tft_code,
         {"TFT operation code", "gtp.tft_code", FT_UINT8, BASE_DEC, VALS(tft_code_type), GTPv1_TFT_CODE_MASK, NULL, HFILL}},
        {&hf_gtp_tft_spare, {"TFT spare bit", "gtp.tft_spare", FT_UINT8, BASE_DEC, NULL, GTPv1_TFT_SPARE_MASK, NULL, HFILL}},
        {&hf_gtp_tft_number,
         {"Number of packet filters", "gtp.tft_number", FT_UINT8, BASE_DEC, NULL, GTPv1_TFT_NUMBER_MASK, NULL, HFILL}},
        {&hf_gtp_tft_eval, {"Evaluation precedence", "gtp.tft_eval", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL}},
        {&hf_gtp_tid, {"TID", "gtp.tid", FT_STRING, BASE_NONE, NULL, 0, "Tunnel Identifier", HFILL}},
        {&hf_gtp_tlli, {"TLLI", "gtp.tlli", FT_UINT32, BASE_HEX, NULL, 0, "Temporary Logical Link Identity", HFILL}},
        {&hf_gtp_tr_comm, {"Packet transfer command", "gtp.tr_comm", FT_UINT8, BASE_DEC, VALS(tr_comm_type), 0, NULL, HFILL}},
        {&hf_gtp_trace_ref, {"Trace reference", "gtp.trace_ref", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_trace_type, {"Trace type", "gtp.trace_type", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL}},
        {&hf_gtp_unknown, {"Unknown data (length)", "gtp.unknown", FT_UINT16, BASE_DEC, NULL, 0, "Unknown data", HFILL}},
        {&hf_gtp_user_addr_pdp_org,
         {"PDP type organization", "gtp.user_addr_pdp_org", FT_UINT8, BASE_DEC, VALS(pdp_org_type), 0, NULL, HFILL}},
        {&hf_gtp_user_addr_pdp_type, {"PDP type number", "gtp.user_addr_pdp_type", FT_UINT8, BASE_HEX, VALS(pdp_type), 0, NULL, HFILL}},
        {&hf_gtp_user_ipv4, {"End user address IPv4", "gtp.user_ipv4", FT_IPv4, BASE_NONE, NULL, 0, NULL, HFILL}},
        {&hf_gtp_user_ipv6, {"End user address IPv6", "gtp.user_ipv6", FT_IPv6, BASE_NONE, NULL, 0, NULL, HFILL}},
        {&hf_gtp_security_mode,
         {"Security Mode", "gtp.security_mode",
          FT_UINT8, BASE_DEC, VALS(mm_sec_modep), 0xc0,
          NULL, HFILL}
        },
        {&hf_gtp_no_of_vectors,
         {"No of Vectors", "gtp.no_of_vectors",
          FT_UINT8, BASE_DEC, NULL, 0x38,
          NULL, HFILL}
        },
        {&hf_gtp_cipher_algorithm,
         {"Cipher Algorithm", "gtp.cipher_algorithm",
          FT_UINT8, BASE_DEC, VALS(gtp_cipher_algorithm), 0x07,
          NULL, HFILL}
        },
        {&hf_gtp_cksn_ksi,
         {"Ciphering Key Sequence Number (CKSN)/Key Set Identifier (KSI)", "gtp.cksn_ksi",
          FT_UINT8, BASE_DEC, NULL, 0x07,
          "CKSN/KSI", HFILL}
        },
        {&hf_gtp_cksn,
         {"Ciphering Key Sequence Number (CKSN)", "gtp.cksn",
          FT_UINT8, BASE_DEC, NULL, 0x07,
          "CKSN", HFILL}
        },
        {&hf_gtp_ksi,
         {"Key Set Identifier (KSI)", "gtp.ksi",
          FT_UINT8, BASE_DEC, NULL, 0x07,
          "KSI", HFILL}
        },
        {&hf_gtp_ext_length,
         {"Length", "gtp.ext_length",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          "IE Length", HFILL}
        },
        {&hf_gtp_ext_apn_res,
         {"Restriction Type", "gtp.ext_apn_res",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_ext_rat_type,
         {"RAT Type", "gtp.ext_rat_type",
          FT_UINT8, BASE_DEC, VALS(gtp_ext_rat_type_vals), 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_ext_geo_loc_type,
         {"Geographic Location Type", "gtp.ext_geo_loc_type",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_ext_sac,
         {"SAC", "gtp.ext_sac",
          FT_UINT16, BASE_HEX, NULL, 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_ext_imeisv,
         {"IMEI(SV)", "gtp.ext_imeisv",
          FT_STRING, BASE_NONE, NULL, 0x0,
          NULL, HFILL}
        },
        { &hf_gtp_targetRNC_ID,
          { "targetRNC-ID", "gtp.targetRNC_ID",
            FT_NONE, BASE_NONE, NULL, 0,
            NULL, HFILL }},
        {&hf_gtp_bssgp_cause,
         {"BSSGP Cause", "gtp.bssgp_cause",
          FT_UINT8, BASE_DEC|BASE_EXT_STRING, &bssgp_cause_vals_ext, 0,
          NULL, HFILL}},
        { &hf_gtp_bssgp_ra_discriminator,
          { "Routing Address Discriminator", "gtp.bssgp.rad",
            FT_UINT8, BASE_DEC, VALS(gtp_bssgp_ra_discriminator_vals), 0x0f,
            NULL, HFILL }
        },
        {&hf_gtp_sapi,
         {"PS Handover XID SAPI", "gtp.ps_handover_xid_sapi",
          FT_UINT8, BASE_DEC, NULL, 0x0F,
          "SAPI", HFILL}},
        {&hf_gtp_xid_par_len,
         {"PS Handover XID parameter length", "gtp.ps_handover_xid_par_len",
          FT_UINT8, BASE_DEC, NULL, 0xFF,
          "XID parameter length", HFILL}},
        {&hf_gtp_earp_pvi,
         {"PVI Pre-emption Vulnerability", "gtp.EARP_pre_emption_par_vulnerability",
          FT_UINT8, BASE_DEC, NULL, 0x01,
          NULL, HFILL}},
        {&hf_gtp_earp_pl,
         {"PL Priority Level", "gtp.EARP_priority_level",
          FT_UINT8, BASE_DEC, NULL, 0x3C,
          NULL, HFILL}},
        {&hf_gtp_earp_pci,
         {"PCI Pre-emption Capability", "gtp.EARP_pre_emption_Capability",
          FT_UINT8, BASE_DEC, NULL, 0x40,
          NULL, HFILL}},
        {&hf_gtp_cdr_app,
         {"Application Identifier", "gtp.cdr_app",
          FT_UINT8, BASE_DEC, NULL, 0xf0,
          NULL, HFILL}},
        { &hf_gtp_cdr_rel,
         {"Release Identifier", "gtp.cdr_rel",
          FT_UINT8, BASE_DEC, NULL, 0x0f,
          NULL, HFILL}},
        { &hf_gtp_cdr_ver,
         {"Version Identifier", "gtp.cdr_ver",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          NULL, HFILL}},
        {&hf_gtp_spare,
         {"Spare", "gtp.spare",
          FT_UINT8, BASE_DEC, NULL, 0x02,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_ppc,
         {"Prohibit Payload Compression", "gtp.cmn_flg.ppc",
          FT_BOOLEAN, 8, NULL, 0x01,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_mbs_srv_type,
         {"MBMS Service Type", "gtp.cmn_flg.mbs_srv_type",
          FT_BOOLEAN, 8, NULL, 0x02,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_mbs_ran_pcd_rdy,
         {"RAN Procedures Ready", "gtp.cmn_flg.mbs_ran_pcd_rdy",
          FT_BOOLEAN, 8, NULL, 0x04,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_mbs_cnt_inf,
         {"MBMS Counting Information", "gtp.cmn_flg.mbs_cnt_inf",
          FT_BOOLEAN, 8, NULL, 0x08,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_no_qos_neg,
         {"No QoS negotiation", "gtp.cmn_flg.no_qos_neg",
          FT_BOOLEAN, 8, NULL, 0x10,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_nrsn,
         {"NRSN bit field", "gtp.cmn_flg.nrsn",
          FT_BOOLEAN, 8, NULL, 0x20,
          NULL, HFILL}},
        {&hf_gtp_cmn_flg_upgrd_qos_sup,
         {"Upgrade QoS Supported", "gtp.cmn_flg.upgrd_qos_sup",
          FT_BOOLEAN, 8, NULL, 0x40,
          NULL, HFILL}},
        {&hf_gtp_tmgi,
         {"Temporary Mobile Group Identity (TMGI)", "gtp.tmgi",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL}},
        {&hf_gtp_no_of_mbms_sa_codes,
         {"Number of MBMS service area codes", "gtp.no_of_mbms_sa_codes",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          "Number N of MBMS service area codes", HFILL}
        },
        {&hf_gtp_mbms_ses_dur_days,
         {"Estimated session duration days", "gtp.mbms_ses_dur_days",
          FT_UINT24, BASE_DEC, NULL, 0x00007F,
          NULL, HFILL}
        },
        {&hf_gtp_mbms_ses_dur_s,
         {"Estimated session duration seconds", "gtp.mbms_ses_dur_s",
          FT_UINT24, BASE_DEC, NULL, 0xFFFF80,
          NULL, HFILL}
        },
        {&hf_gtp_mbms_sa_code,
         {"MBMS service area code", "gtp.mbms_sa_code",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_mbs_2g_3g_ind,
         {"MBMS 2G/3G Indicator", "gtp.mbs_2g_3g_ind",
          FT_UINT8, BASE_DEC, VALS(gtp_mbs_2g_3g_ind_vals), 0x0,
          NULL, HFILL}
        },
        {&hf_gtp_time_2_dta_tr,
         {"Time to MBMS Data Transfer", "gtp.time_2_dta_tr",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
        { &hf_gtp_ext_ei,
          {"Error Indication (EI)", "gtp.ei",
           FT_UINT8, BASE_DEC, NULL, 0x04,
           NULL, HFILL}
        },
        {&hf_gtp_ext_gcsi,
         {"GPRS-CSI (GCSI)", "gtp.gcsi",
          FT_UINT8, BASE_DEC, NULL, 0x02,
          NULL, HFILL}
        },
        { &hf_gtp_ext_dti,
          {"Direct Tunnel Indicator (DTI)", "gtp.dti",
           FT_UINT8, BASE_DEC, NULL, 0x01,
           NULL, HFILL}
        },
        { &hf_gtp_ra_prio_lcs,
          {"Radio Priority LCS", "gtp.raplcs",
           FT_UINT8, BASE_DEC, NULL, 0x07,
           NULL, HFILL}
        },
        { &hf_gtp_bcm,
         {"Bearer Control Mode", "gtp.bcm",
          FT_UINT8, BASE_DEC, VALS(gtp_pdp_bcm_type_vals), 0,
          NULL, HFILL}
        },
        { &hf_gtp_fqdn,
         {"FQDN", "gtp.fqdn",
          FT_STRING, BASE_NONE, NULL, 0,
          "Fully Qualified Domain Name", HFILL}
        },
        { &hf_gtp_rim_routing_addr,
         {"RIM Routing Address value", "gtp.rim_routing_addr_val",
          FT_BYTES, BASE_NONE, NULL, 0,
          NULL, HFILL}
        },
		{ &hf_gtp_mbms_flow_id,
         {"MBMS Flow Identifier", "gtp.mbms_flow_id",
          FT_BYTES, BASE_NONE, NULL, 0,
          NULL, HFILL}
        },
		{ &hf_gtp_mbms_dist_indic,
         {"Distribution Indication", "gtp.mbms_dist_indic",
          FT_UINT8, BASE_DEC, VALS(gtp_mbms_dist_indic_vals), 0x03,
          NULL, HFILL}
        },
		{ &hf_gtp_ext_apn_ambr_ul,
         {"APN-AMBR for Uplink", "gtp.apn_ambr_ul",
          FT_INT32, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
		{ &hf_gtp_ext_apn_ambr_dl,
         {"APN-AMBR for Downlink", "gtp.apn_ambr_dl",
          FT_INT32, BASE_DEC, NULL, 0x0,
          NULL, HFILL}
        },
    };

	/
#define GTP_NUM_INDIVIDUAL_ELEMS    65
    static gint *ett_gtp_array[GTP_NUM_INDIVIDUAL_ELEMS + NUM_GTP_IES];

    ett_gtp_array[0] = &ett_gtp;
    ett_gtp_array[1] = &ett_gtp_flags;
    ett_gtp_array[2] = &ett_gtp_ext;
    ett_gtp_array[3] = &ett_gtp_rai;
    ett_gtp_array[4] = &ett_gtp_qos;
    ett_gtp_array[5] = &ett_gtp_auth_tri;
    ett_gtp_array[6] = &ett_gtp_flow_ii;
    ett_gtp_array[7] = &ett_gtp_rab_cntxt;
    ett_gtp_array[8] = &ett_gtp_rp;
    ett_gtp_array[9] = &ett_gtp_pkt_flow_id;
    ett_gtp_array[10] = &ett_gtp_chrg_char;
    ett_gtp_array[11] = &ett_gtp_user;
    ett_gtp_array[12] = &ett_gtp_mm;
    ett_gtp_array[13] = &ett_gtp_trip;
    ett_gtp_array[14] = &ett_gtp_quint;
    ett_gtp_array[15] = &ett_gtp_pdp;
    ett_gtp_array[16] = &ett_gtp_apn;
    ett_gtp_array[17] = &ett_gtp_proto;
    ett_gtp_array[18] = &ett_gtp_gsn_addr;
    ett_gtp_array[19] = &ett_gtp_tft;
    ett_gtp_array[20] = &ett_gtp_tft_pf;
    ett_gtp_array[21] = &ett_gtp_tft_flags;
    ett_gtp_array[22] = &ett_gtp_rab_setup;
    ett_gtp_array[23] = &ett_gtp_hdr_list;
    ett_gtp_array[24] = &ett_gtp_chrg_addr;
    ett_gtp_array[25] = &ett_gtp_node_addr;
    ett_gtp_array[26] = &ett_gtp_rel_pack;
    ett_gtp_array[27] = &ett_gtp_can_pack;
    ett_gtp_array[28] = &ett_gtp_data_resp;
    ett_gtp_array[29] = &ett_gtp_priv_ext;
    ett_gtp_array[30] = &ett_gtp_net_cap;
    ett_gtp_array[31] = &ett_gtp_ext_tree_apn_res;
    ett_gtp_array[32] = &ett_gtp_ext_rat_type;
    ett_gtp_array[33] = &ett_gtp_ext_imeisv;
    ett_gtp_array[34] = &ett_gtp_ext_ran_tr_cont;
    ett_gtp_array[35] = &ett_gtp_ext_pdp_cont_prio;
    ett_gtp_array[36] = &ett_gtp_ext_ssgn_no;
    ett_gtp_array[37] = &ett_gtp_ext_rab_setup_inf;
    ett_gtp_array[38] = &ett_gtp_ext_common_flgs;
    ett_gtp_array[39] = &ett_gtp_ext_usr_loc_inf;
    ett_gtp_array[40] = &ett_gtp_ext_ms_time_zone;
    ett_gtp_array[41] = &ett_gtp_ext_camel_chg_inf_con;
    ett_gtp_array[42] = &ett_GTP_EXT_MBMS_UE_CTX;
    ett_gtp_array[43] = &ett_gtp_ext_tmgi;
    ett_gtp_array[44] = &ett_gtp_tmgi;
    ett_gtp_array[45] = &ett_gtp_ext_rim_ra;
    ett_gtp_array[46] = &ett_gtp_ext_mbms_prot_conf_opt;
    ett_gtp_array[47] = &ett_gtp_ext_mbms_sa;
    ett_gtp_array[48] = &ett_gtp_ext_bms_ses_dur;
    ett_gtp_array[49] = &ett_gtp_ext_src_rnc_pdp_ctx_inf;
    ett_gtp_array[50] = &ett_gtp_ext_add_trs_inf;
    ett_gtp_array[51] = &ett_gtp_ext_hop_count;
    ett_gtp_array[52] = &ett_gtp_ext_sel_plmn_id;
    ett_gtp_array[53] = &ett_gtp_ext_mbms_ses_id;
    ett_gtp_array[54] = &ett_gtp_ext_mbms_2g_3g_ind;
    ett_gtp_array[55] = &ett_gtp_ext_enh_nsapi;
    ett_gtp_array[56] = &ett_gtp_ext_ad_mbms_trs_inf;
    ett_gtp_array[57] = &ett_gtp_ext_mbms_ses_id_rep_no;
    ett_gtp_array[58] = &ett_gtp_ext_ps_handover_xid;
    ett_gtp_array[59] = &ett_gtp_ext_ra_prio_lcs;
    ett_gtp_array[60] = &ett_gtp_ext_hdr;
    ett_gtp_array[61] = &ett_gtp_uli_rai;
    ett_gtp_array[62] = &ett_gtp_cdr_dr;
    ett_gtp_array[63] = &ett_gtp_cdr_ver;

    last_offset = GTP_NUM_INDIVIDUAL_ELEMS;

    for (i=0; i < NUM_GTP_IES; i++, last_offset++)
    {
        ett_gtp_ies[i] = -1;
        ett_gtp_array[last_offset] = &ett_gtp_ies[i];
    }

#if 0
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
        &ett_gtp_ext_tree_apn_res,
        &ett_gtp_ext_rat_type,
        &ett_gtp_ext_imeisv,
        &ett_gtp_ext_ran_tr_cont,
        &ett_gtp_ext_pdp_cont_prio,
        &ett_gtp_ext_ssgn_no,
        &ett_gtp_ext_rab_setup_inf,
        &ett_gtp_ext_common_flgs,
        &ett_gtp_ext_usr_loc_inf,
        &ett_gtp_ext_ms_time_zone,
        &ett_gtp_ext_camel_chg_inf_con,
        &ett_GTP_EXT_MBMS_UE_CTX,
        &ett_gtp_ext_tmgi,
        &ett_gtp_tmgi,
        &ett_gtp_ext_rim_ra,
        &ett_gtp_ext_mbms_prot_conf_opt,
        &ett_gtp_ext_mbms_sa,
        &ett_gtp_ext_bms_ses_dur,
        &ett_gtp_ext_src_rnc_pdp_ctx_inf,
        &ett_gtp_ext_add_trs_inf,
        &ett_gtp_ext_hop_count,
        &ett_gtp_ext_sel_plmn_id,
        &ett_gtp_ext_mbms_ses_id,
        &ett_gtp_ext_mbms_2g_3g_ind,
        &ett_gtp_ext_enh_nsapi,
        &ett_gtp_ext_ad_mbms_trs_inf,
        &ett_gtp_ext_mbms_ses_id_rep_no,
        &ett_gtp_ext_ra_prio_lcs,
        &ett_gtp_ext_ps_handover_xid,
        &ett_gtp_target_id,
        &ett_gtp_utran_cont,
        &ett_gtp_bcm,
        &ett_gtp_fqdn,
        &ett_gtp_cdr_ver,
        &ett_gtp_cdr_dr,
        &ett_gtp_ext_hdr,
        &ett_gtp_uli_rai,
		&ett_gtp_ext_ue_network_cap,
    };
#endif


    proto_gtp = proto_register_protocol("GPRS Tunneling Protocol", "GTP", "gtp");
    proto_register_field_array(proto_gtp, hf_gtp, array_length(hf_gtp));
    proto_register_subtree_array(ett_gtp_array, array_length(ett_gtp_array));

    gtp_module = prefs_register_protocol(proto_gtp, proto_reg_handoff_gtp);

    prefs_register_uint_preference(gtp_module, "v0_port", "GTPv0 and GTP' port", "GTPv0 and GTP' port (default 3386)", 10, &g_gtpv0_port);
    prefs_register_uint_preference(gtp_module, "v1c_port", "GTPv1 or GTPv2 control plane (GTP-C, GTPv2-C) port", "GTPv1 and GTPv2 control plane port (default 2123)", 10,
                                   &g_gtpv1c_port);
    prefs_register_uint_preference(gtp_module, "v1u_port", "GTPv1 user plane (GTP-U) port", "GTPv1 user plane port (default 2152)", 10,
                                   &g_gtpv1u_port);
    prefs_register_bool_preference(gtp_module, "dissect_tpdu", "Dissect T-PDU", "Dissect T-PDU", &g_gtp_tpdu);

    prefs_register_obsolete_preference(gtp_module, "v0_dissect_cdr_as");
    prefs_register_obsolete_preference(gtp_module, "v0_check_etsi");
    prefs_register_obsolete_preference(gtp_module, "v1_check_etsi");
    prefs_register_bool_preference(gtp_module, "check_etsi", "Compare GTP order with ETSI", "GTP ETSI order", &g_gtp_etsi_order);
    prefs_register_obsolete_preference(gtp_module, "ppp_reorder");

    /
    prefs_register_bool_preference(gtp_module, "dissect_gtp_over_tcp", "Dissect GTP over TCP", "Dissect GTP over TCP", &g_gtp_over_tcp);

    register_dissector("gtp", dissect_gtp, proto_gtp);
    register_dissector("gtpprim", dissect_gtpprim, proto_gtp);

    gtp_priv_ext_dissector_table = register_dissector_table("gtp.priv_ext", "GTP PRIVATE EXT", FT_UINT16, BASE_DEC);
    gtp_cdr_fmt_dissector_table = register_dissector_table("gtp.cdr_fmt", "GTP DATA RECORD TYPE", FT_UINT16, BASE_DEC);

    register_init_routine(gtp_reinit);
    gtp_tap=register_tap("gtp");
}
