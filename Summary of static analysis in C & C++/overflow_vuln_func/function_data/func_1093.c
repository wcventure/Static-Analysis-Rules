void
proto_register_iec104asdu(void)
{
	static hf_register_info hf_as[] = {

		{ &hf_addr,
		  { "Addr", "104asdu.addr", FT_UINT16, BASE_DEC, NULL, 0x0,
		    "Common Address of Asdu", HFILL }},

		{ &hf_oa,
		  { "OA", "104asdu.oa", FT_UINT8, BASE_DEC, NULL, 0x0,
		    "Originator Address", HFILL }},

		{ &hf_typeid,
		  { "TypeId", "104asdu.typeid", FT_UINT8, BASE_DEC, VALS(asdu_types), 0x0,
		    "Asdu Type Id", HFILL }},

		{ &hf_causetx,
		  { "CauseTx", "104asdu.causetx", FT_UINT8, BASE_DEC, VALS(causetx_types), F_CAUSE,
		    "Cause of Transmision", HFILL }},

		{ &hf_nega,
		  { "Negative", "104asdu.nega", FT_BOOLEAN, 8, NULL, F_NEGA,
		    NULL, HFILL }},

		{ &hf_test,
		  { "Test", "104asdu.test", FT_BOOLEAN, 8, NULL, F_TEST,
		    NULL, HFILL }},

		{ &hf_ioa,
		  { "IOA", "104asdu.ioa", FT_UINT24, BASE_DEC, NULL, 0x0,
		    "Information Object Address", HFILL }},

		{ &hf_numix,
		  { "NumIx", "104asdu.numix", FT_UINT8, BASE_DEC, NULL, 0x7F,
		    "Number of Information Objects/Elements", HFILL }},

		{ &hf_sq,
		  { "SQ", "104asdu.sq", FT_BOOLEAN, 8, NULL, F_SQ,
		    "Sequence", HFILL }},

		{ &hf_cp56time,
		  { "CP56Time", "104asdu.cp56time", FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL, NULL, 0,
		    NULL, HFILL }},

		{ &hf_cp56time_ms,
		  { "MS", "104asdu.cp56time.ms", FT_UINT16, BASE_DEC, NULL, 0xFFFF,
		    "CP56Time milliseconds", HFILL }},

		{ &hf_cp56time_min,
		  { "Min", "104asdu.cp56time.min", FT_UINT8, BASE_DEC, NULL, 0x3F,
		    "CP56Time minutes", HFILL }},

		{ &hf_cp56time_iv,
		  { "IV", "104asdu.cp56time.iv", FT_BOOLEAN, 8, TFS(&tfs_invalid_valid), 0x80,
		    "CP56Time invalid", HFILL }},

		{ &hf_cp56time_hour,
		  { "Hour", "104asdu.cp56time.hour", FT_UINT8, BASE_DEC, NULL, 0x1F,
		    "CP56Time hours", HFILL }},

		{ &hf_cp56time_su,
		  { "SU", "104asdu.cp56time.su", FT_BOOLEAN, 8, TFS(&tfs_local_dst), 0x80,
		    "CP56Time summer time", HFILL }},

		{ &hf_cp56time_day,
		  { "Day", "104asdu.cp56time.day", FT_UINT8, BASE_DEC, NULL, 0x1F,
		    "CP56Time day", HFILL }},

		{ &hf_cp56time_dow,
		  { "DOW", "104asdu.cp56time.dow", FT_UINT8, BASE_DEC, NULL, 0xE0,
		    "CP56Time day of week", HFILL }},

		{ &hf_cp56time_month,
		  { "Month", "104asdu.cp56time.month", FT_UINT8, BASE_DEC, NULL, 0x0F,
		    "CP56Time month", HFILL }},

		{ &hf_cp56time_year,
		  { "Year", "104asdu.cp56time.year", FT_UINT8, BASE_DEC, NULL, 0x7F,
		    "CP56Time year", HFILL }},

		{ &hf_siq,
		  { "SIQ", "104asdu.siq", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_siq_spi,
		  { "SPI", "104asdu.siq.spi", FT_BOOLEAN, 8, TFS(&tfs_on_off), 0x01,
		    "SIQ SPI", HFILL }},

		{ &hf_siq_bl,
		  { "BL", "104asdu.siq.bl", FT_BOOLEAN, 8, TFS(&tfs_blocked_not_blocked), 0x10,
		    "SIQ BL", HFILL }},

		{ &hf_siq_sb,
		  { "SB", "104asdu.siq.sb", FT_BOOLEAN, 8, TFS(&tfs_substituted_not_substituted), 0x20,
		    "SIQ SB", HFILL }},

		{ &hf_siq_nt,
		  { "NT", "104asdu.siq.nt", FT_BOOLEAN, 8, TFS(&tfs_not_topical_topical), 0x40,
		    "SIQ NT", HFILL }},

		{ &hf_siq_iv,
		  { "IV", "104asdu.siq.iv", FT_BOOLEAN, 8, TFS(&tfs_invalid_valid), 0x80,
		    "SIQ IV", HFILL }},

		{ &hf_diq,
		  { "DIQ", "104asdu.diq", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_diq_dpi,
		  { "DPI", "104asdu.diq.dpi", FT_UINT8, BASE_DEC, VALS(diq_types), 0x03,
		    "DIQ DPI", HFILL }},

		{ &hf_diq_bl,
		  { "BL", "104asdu.diq.bl", FT_BOOLEAN, 8, TFS(&tfs_blocked_not_blocked), 0x10,
		    "DIQ BL", HFILL }},

		{ &hf_diq_sb,
		  { "SB", "104asdu.diq.sb", FT_BOOLEAN, 8, TFS(&tfs_substituted_not_substituted), 0x20,
		    "DIQ SB", HFILL }},

		{ &hf_diq_nt,
		  { "NT", "104asdu.diq.nt", FT_BOOLEAN, 8, TFS(&tfs_not_topical_topical), 0x40,
		    "DIQ NT", HFILL }},

		{ &hf_diq_iv,
		  { "IV", "104asdu.diq.iv", FT_BOOLEAN, 8, TFS(&tfs_invalid_valid), 0x80,
		    "DIQ IV", HFILL }},

		{ &hf_qds,
		  { "QDS", "104asdu.qds", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_qds_ov,
		  { "OV", "104asdu.qds.ov", FT_BOOLEAN, 8, TFS(&tfs_overflow_no_overflow), 0x01,
		    "QDS OV", HFILL }},

		{ &hf_qds_bl,
		  { "BL", "104asdu.qds.bl", FT_BOOLEAN, 8, TFS(&tfs_blocked_not_blocked), 0x10,
		    "QDS BL", HFILL }},

		{ &hf_qds_sb,
		  { "SB", "104asdu.qds.sb", FT_BOOLEAN, 8, TFS(&tfs_substituted_not_substituted), 0x20,
		    "QDS SB", HFILL }},

		{ &hf_qds_nt,
		  { "NT", "104asdu.qds.nt", FT_BOOLEAN, 8, TFS(&tfs_not_topical_topical), 0x40,
		    "QDS NT", HFILL }},

		{ &hf_qds_iv,
		  { "IV", "104asdu.qds.iv", FT_BOOLEAN, 8, TFS(&tfs_invalid_valid), 0x80,
		    "QDS IV", HFILL }},

		{ &hf_vti,
		  { "VTI", "104asdu.vti", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_vti_v,
		  { "Value", "104asdu.vti.v", FT_INT8, BASE_DEC, NULL, 0x7F,
		    "VTI Value", HFILL }},

		{ &hf_vti_t,
		  { "T", "104asdu.vti.t", FT_BOOLEAN, 8, TFS(&tfs_transient_not_transient), 0x80,
		    "VTI T", HFILL }},

		{ &hf_qos,
		  { "QOS", "104asdu.qos", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_qos_ql,
		  { "QL", "104asdu.qos.ql", FT_UINT8, BASE_DEC, NULL, 0x7F,
		    "QOS QL", HFILL }},

		{ &hf_qos_se,
		  { "S/E", "104asdu.qos.se", FT_BOOLEAN, 8, TFS(&tfs_select_execute), 0x80,
		    "QOS S/E", HFILL }},

		{ &hf_sco,
		  { "SCO", "104asdu.sco", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_sco_on,
		  { "ON/OFF", "104asdu.sco.on", FT_BOOLEAN, 8, TFS(&tfs_on_off), 0x01,
		    "SCO SCS", HFILL }},

		{ &hf_sco_qu,
		  { "QU", "104asdu.sco.qu", FT_UINT8, BASE_DEC, VALS(qos_qu_types), 0x7C,
		    "SCO QU", HFILL }},

		{ &hf_sco_se,
		  { "S/E", "104asdu.sco.se", FT_BOOLEAN, 8, TFS(&tfs_select_execute), 0x80,
		    "SCO S/E", HFILL }},

		{ &hf_dco,
		  { "DCO", "104asdu.dco", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_dco_on,
		  { "ON/OFF", "104asdu.dco.on", FT_UINT8, BASE_DEC, VALS(dco_on_types), 0x03,
		    "DCO DCS", HFILL }},

		{ &hf_dco_qu,
		  { "QU", "104asdu.dco.qu", FT_UINT8, BASE_DEC, VALS(qos_qu_types), 0x7C,
		    "DCO QU", HFILL }},

		{ &hf_dco_se,
		  { "S/E", "104asdu.dco.se", FT_BOOLEAN, 8, TFS(&tfs_select_execute), 0x80,
		    "DCO S/E", HFILL }},

		{ &hf_rco,
		  { "RCO", "104asdu.rco", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_rco_up,
		  { "UP/DOWN", "104asdu.rco.up", FT_UINT8, BASE_DEC, VALS(rco_up_types), 0x03,
		    "RCO RCS", HFILL }},

		{ &hf_rco_qu,
		  { "QU", "104asdu.rco.qu", FT_UINT8, BASE_DEC, VALS(qos_qu_types), 0x7C,
		    "RCO QU", HFILL }},

		{ &hf_rco_se,
		  { "S/E", "104asdu.rco.se", FT_BOOLEAN, 8, TFS(&tfs_select_execute), 0x80,
		    "RCO S/E", HFILL }},

		{ &hf_coi,
		  { "COI", "104asdu.coi", FT_UINT8, BASE_HEX, NULL, 0,
		    NULL, HFILL }},

		{ &hf_coi_r,
		  { "R", "104asdu.coi_r", FT_UINT8, BASE_DEC, VALS(coi_r_types), 0x7F,
		    "COI R", HFILL }},

		{ &hf_coi_i,
		  { "I", "104asdu.coi_i", FT_BOOLEAN, 8, TFS(&tfs_coi_i), 0x80,
		    "COI I", HFILL }},

		{ &hf_qoi,
		  { "QOI", "104asdu.qoi", FT_UINT8, BASE_DEC, VALS(qoi_r_types), 0,
		    NULL, HFILL }},

		{ &hf_bcr_count,
		  { "Binary Counter", "104asdu.bcr.count", FT_INT32, BASE_DEC, NULL, 0x0,
		    NULL, HFILL }},

		{ &hf_bcr_sq,
		  { "SQ", "104asdu.bcr.sq", FT_UINT8, BASE_DEC, NULL, 0x1F,
		    "Sequence Number", HFILL }},

		{ &hf_bcr_cy,
		  { "CY", "104asdu.bcr.cy", FT_BOOLEAN, 8, TFS(&tfs_overflow), 0x20,
		    "Counter Overflow", HFILL }},

		{ &hf_bcr_ca,
		  { "CA", "104asdu.bcr.ca", FT_BOOLEAN, 8, TFS(&tfs_adjusted), 0x40,
		    "Counter Adjusted", HFILL }},

		{ &hf_bcr_iv,
		  { "IV", "104asdu.bcr.iv", FT_BOOLEAN, 8, TFS(&tfs_invalid_valid), 0x80,
		    "Counter Validity", HFILL }},

		{ &hf_asdu_bitstring,
		  { "Value", "104asdu.bitstring", FT_UINT32, BASE_HEX, NULL, 0x0,
		    "BSI value", HFILL }},

		{ &hf_asdu_float,
		  { "Value", "104asdu.float", FT_FLOAT, BASE_NONE, NULL, 0x0,
		    "Float value", HFILL }},

		{ &hf_asdu_normval,
		  { "Value", "104asdu.normval", FT_FLOAT, BASE_NONE, NULL, 0x0,
		    "Normalised value", HFILL }},

		{ &hf_asdu_scalval,
		  { "Value", "104asdu.scalval", FT_INT16, BASE_DEC, NULL, 0x0,
		    "Scaled value", HFILL }},
	};

	static gint *ett_as[] = {
		&ett_asdu,
		&ett_asdu_objects,
		&ett_siq,
		&ett_diq,
		&ett_qds,
		&ett_qos,
		&ett_vti,
		&ett_sco,
		&ett_dco,
		&ett_rco,
		&ett_cp56time
	};

	static ei_register_info ei[] = {
		{ &ei_iec104_short_asdu, { "iec104.short_asdu", PI_MALFORMED, PI_ERROR, "<ERR Short Asdu>", EXPFILL }},
		{ &ei_iec104_apdu_min_len, { "iec104.apdu_min_len", PI_MALFORMED, PI_ERROR, "APDU less than bytes", EXPFILL }},
	};

	expert_module_t* expert_iec104;

	proto_iec104asdu = proto_register_protocol(
		"IEC 60870-5-104-Asdu",
		"104asdu",
		"104asdu"
		);

	proto_register_field_array(proto_iec104asdu, hf_as, array_length(hf_as));
	proto_register_subtree_array(ett_as, array_length(ett_as));
	expert_iec104 = expert_register_protocol(proto_iec104asdu);
	expert_register_field_array(expert_iec104, ei, array_length(ei));
}
