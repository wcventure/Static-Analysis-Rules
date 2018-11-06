static void dissect_iec104asdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint Len = tvb_reported_length(tvb);
	guint8 Bytex;
	const char *cause_str;
	size_t Ind;
	struct asduheader asduh;
	proto_item *it104;
	proto_tree *it104tree;
	wmem_strbuf_t * res;

	guint8 offset = 0;  /
	guint8 offset_start_ioa = 0; /
	guint8 i;
	guint32 asdu_info_obj_addr = 0;
	proto_item * itSignal = NULL;
	proto_tree * trSignal;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "104asdu");

	it104 = proto_tree_add_item(tree, proto_iec104asdu, tvb, 0, -1, ENC_NA);
	it104tree = proto_item_add_subtree(it104, ett_asdu);

	res = wmem_strbuf_new_label(wmem_packet_scope());

	/
	asduh.TypeId = tvb_get_guint8(tvb, 0);
	proto_tree_add_item(it104tree, hf_typeid, tvb, 0, 1, ENC_LITTLE_ENDIAN);
	asduh.DataLength = get_TypeIdLength(asduh.TypeId);

	/
	Bytex = tvb_get_guint8(tvb, 1);
	asduh.SQ = Bytex & F_SQ;
	asduh.NumIx = Bytex & 0x7F;
	proto_tree_add_item(it104tree, hf_sq, tvb, 1, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(it104tree, hf_numix, tvb, 1, 1, ENC_LITTLE_ENDIAN);

	/
	asduh.TNCause = tvb_get_guint8(tvb, 2);
	proto_tree_add_item(it104tree, hf_causetx, tvb, 2, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(it104tree, hf_nega, tvb, 2, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(it104tree, hf_test, tvb, 2, 1, ENC_LITTLE_ENDIAN);

	/
	asduh.OA = tvb_get_guint8(tvb, 3);
	proto_tree_add_item(it104tree, hf_oa, tvb, 3, 1, ENC_LITTLE_ENDIAN);

	/
	asduh.Addr = tvb_get_letohs(tvb, 4);
	proto_tree_add_item(it104tree, hf_addr, tvb, 4, 2, ENC_LITTLE_ENDIAN);

	/
	asduh.IOA = tvb_get_letoh24(tvb, 6);

	cause_str = val_to_str(asduh.TNCause & F_CAUSE, causetx_types, " <CauseTx=%u>");

	wmem_strbuf_append_printf(res, "ASDU=%u %s %s", asduh.Addr, val_to_str(asduh.TypeId, asdu_types, "<TypeId=%u>"), cause_str);

	if (asduh.TNCause & F_NEGA)
		wmem_strbuf_append(res, "_NEGA");
	if (asduh.TNCause & F_TEST)
		wmem_strbuf_append(res, "_TEST");

	if ((asduh.TNCause & (F_TEST | F_NEGA)) == 0) {
		for (Ind=strlen(cause_str); Ind< 7; Ind++)
			wmem_strbuf_append(res, " ");
	}

	if (asduh.NumIx > 1) {
		wmem_strbuf_append_printf(res, " IOA[%d]=%d", asduh.NumIx, asduh.IOA);
		if (asduh.SQ == F_SQ)
			wmem_strbuf_append_printf(res, "-%d", asduh.IOA + asduh.NumIx - 1);
		else
			wmem_strbuf_append(res, ",...");
	} else {
		wmem_strbuf_append_printf(res, " IOA=%d", asduh.IOA);
	}

	col_append_str(pinfo->cinfo, COL_INFO, wmem_strbuf_get_str(res));
	col_set_fence(pinfo->cinfo, COL_INFO);

	/
	proto_item_append_text(it104, ": %s '%s'", wmem_strbuf_get_str(res),
		Len >= ASDU_HEAD_LEN ? val_to_str_const(asduh.TypeId, asdu_lngtypes, "<Unknown TypeId>") : "");

	/
	offset = 6;  /
	/

	switch (asduh.TypeId) {
		case M_SP_NA_1:
		case M_DP_NA_1:
		case M_ST_NA_1:
		case M_BO_NA_1:
		case M_SP_TB_1:
		case M_DP_TB_1:
		case M_ST_TB_1:
		case M_BO_TB_1:
		case M_ME_NA_1:
		case M_ME_NB_1:
		case M_ME_NC_1:
		case M_ME_ND_1:
		case M_ME_TD_1:
		case M_ME_TE_1:
		case M_ME_TF_1:
		case M_IT_TB_1:
		case C_SC_NA_1:
		case C_DC_NA_1:
		case C_RC_NA_1:
		case C_SE_NA_1:
		case C_SE_NB_1:
		case C_SE_NC_1:
		case C_BO_NA_1:
		case C_SC_TA_1:
		case C_DC_TA_1:
		case C_RC_TA_1:
		case C_SE_TA_1:
		case C_SE_TB_1:
		case C_SE_TC_1:
		case C_BO_TA_1:
		case M_EI_NA_1:
		case C_IC_NA_1:
		case C_CS_NA_1:

			/
			for(i = 0; i < asduh.NumIx; i++)
			{
				/
				itSignal = proto_tree_add_text(it104tree, tvb, offset, asduh.DataLength + 3, "IOA:s");
				trSignal = proto_item_add_subtree(itSignal, ett_asdu_objects);

				/
				if (!i)
				{
					offset_start_ioa = offset;
					/
					/
					if(Len < (guint)(offset + 3)) {
						expert_add_info(pinfo, itSignal, &ei_iec104_short_asdu);
						return;
					}
					get_InfoObjectAddress(&asdu_info_obj_addr, tvb, &offset, trSignal);
				} else {
					/
					if (asduh.SQ) /
					{
						asdu_info_obj_addr++;
						proto_tree_add_uint(trSignal, hf_ioa, tvb, offset_start_ioa, 3, asdu_info_obj_addr);
					} else { /
						/
						/
						if(Len < (guint)(offset + 3)) {
							expert_add_info(pinfo, itSignal, &ei_iec104_short_asdu);
							return;
						}
						get_InfoObjectAddress(&asdu_info_obj_addr, tvb, &offset, trSignal);
					}
				}

				proto_item_set_text(itSignal, "IOA: %d", asdu_info_obj_addr);

				/
				if(Len < (guint)(offset + asduh.DataLength)) {
					expert_add_info(pinfo, itSignal, &ei_iec104_short_asdu);
					return;
				}

				switch (asduh.TypeId) {
				case M_SP_NA_1: /
					get_SIQ(tvb, &offset, trSignal);
					break;
				case M_DP_NA_1: /
					get_DIQ(tvb, &offset, trSignal);
					break;
				case M_ST_NA_1: /
					get_VTI(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					break;
				case M_BO_NA_1: /
					get_BSI(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					break;
				case M_ME_NA_1: /
					get_NVA(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					break;
				case M_ME_NB_1: /
					get_SVA(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					break;
				case M_ME_NC_1: /
					get_FLT(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					break;
				case M_ME_ND_1: /
					get_NVA(tvb, &offset, trSignal);
					break;
				case M_SP_TB_1: /
					get_SIQ(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_DP_TB_1: /
					get_DIQ(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_ST_TB_1: /
					get_VTI(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_BO_TB_1: /
					get_BSI(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_ME_TD_1: /
					get_NVA(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_ME_TE_1: /
					get_SVA(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_ME_TF_1: /
					get_FLT(tvb, &offset, trSignal);
					get_QDS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_IT_TB_1: /
					get_BCR(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_SC_NA_1: /
					get_SCO(tvb, &offset, trSignal);
					break;
				case C_DC_NA_1: /
					get_DCO(tvb, &offset, trSignal);
					break;
				case C_RC_NA_1: /
					get_RCO(tvb, &offset, trSignal);
					break;
				case C_SE_NA_1: /
					get_NVAspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					break;
				case C_SE_NB_1: /
					get_SVAspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					break;
				case C_SE_NC_1: /
					get_FLTspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					break;
				case C_BO_NA_1: /
					get_BSIspt(tvb, &offset, trSignal);
					break;
				case C_SC_TA_1: /
					get_SCO(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_DC_TA_1: /
					get_DCO(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_RC_TA_1: /
					get_RCO(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_SE_TA_1: /
					get_NVAspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_SE_TB_1: /
					get_SVAspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_SE_TC_1: /
					get_FLTspt(tvb, &offset, trSignal);
					get_QOS(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case C_BO_TA_1: /
					get_BSIspt(tvb, &offset, trSignal);
					get_CP56Time(tvb, &offset, trSignal);
					break;
				case M_EI_NA_1: /
					get_COI(tvb, &offset, trSignal);
					break;
				case C_IC_NA_1: /
					get_QOI(tvb, &offset, trSignal);
					break;
				case C_CS_NA_1: /
					get_CP56Time(tvb, &offset, trSignal);
					break;

				default:
    				break;
				} /
			} /
			break;
		default:
			proto_tree_add_item(it104tree, hf_ioa, tvb, offset, 3, ENC_LITTLE_ENDIAN);
			break;
	} /

}
