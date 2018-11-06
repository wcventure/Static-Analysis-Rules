static void
dissect_decttype(gboolean dect_packet_type, struct dect_afield *pkt_afield,
	struct dect_bfield *pkt_bfield, packet_info *pinfo, const guint8 *pkt_ptr,
	tvbuff_t *tvb, proto_item *ti, proto_tree *DectTree)
{
	guint16 rcrc;
	guint8 rcrcdat[8];
	gint offset			= DECT_PACKET_INFO_LEN;
	guint8 tailtype		= 0;
	emem_strbuf_t *afield_str;

	proto_tree *ColumnsTree = NULL;
	proto_item *columnstreeti = NULL;

	proto_item *afieldti	= NULL;
	proto_item *aheadti	= NULL;
	proto_item *atailti	= NULL;
	proto_tree *AField	= NULL;
	proto_tree *AHead	= NULL;
	proto_tree *ATail	= NULL;

	guint8	header, tail_0, tail_1, tail_2, tail_3, tail_4;

	/
	columnstreeti	= proto_tree_add_item(DectTree, hf_dect_cc, tvb,0,0, FALSE);
	ColumnsTree		= proto_item_add_subtree(columnstreeti, ett_afield);

	afield_str		= ep_strbuf_new(NULL);

	col_append_fstr(pinfo->cinfo, COL_INFO, "Use Custom Columns for Infos");

	/

	/
	header = tvb_get_guint8(tvb, offset+0);
	tail_0 = tvb_get_guint8(tvb, offset+1);
	tail_1 = tvb_get_guint8(tvb, offset+2);
	tail_2 = tvb_get_guint8(tvb, offset+3);
	tail_3 = tvb_get_guint8(tvb, offset+4);
	tail_4 = tvb_get_guint8(tvb, offset+5);
	tailtype	= header >> 5;
	afieldti	= proto_tree_add_item(DectTree, hf_dect_A, tvb, offset, 8, FALSE);
	AField		= proto_item_add_subtree(afieldti, ett_afield);

	/
	aheadti		= proto_tree_add_item(afieldti, hf_dect_A_Head, tvb, offset, 1, FALSE);
	AHead		= proto_item_add_subtree(aheadti, ett_ahead);

	if(dect_packet_type==DECT_PACKET_FP)
		proto_tree_add_item(AHead, hf_dect_A_Head_TA_FP, tvb, offset, 1, FALSE);
	else
		proto_tree_add_item(AHead, hf_dect_A_Head_TA_PP, tvb, offset, 1, FALSE);

	proto_tree_add_item(AHead, hf_dect_A_Head_Q1, tvb, offset, 1, FALSE);
	proto_tree_add_item(AHead, hf_dect_A_Head_BA, tvb, offset, 1, FALSE);
	proto_tree_add_item(AHead, hf_dect_A_Head_Q2, tvb, offset, 1, FALSE);
	offset++;

	/
	if(dect_packet_type==DECT_PACKET_FP)
	{
		atailti = proto_tree_add_none_format(afieldti, hf_dect_A_Tail, tvb, offset, 5, 
			"FP-Tail: %s", val_to_str(tailtype, TA_vals_FP, "Error, please report: %d"));
	}
	else
	{
		atailti = proto_tree_add_none_format(afieldti, hf_dect_A_Tail, tvb, offset, 5, 
			"PP-Tail: %s", val_to_str(tailtype, TA_vals_PP, "Error, please report: %d"));
	}

	ATail = proto_item_add_subtree(atailti, ett_atail);

	if((tailtype==0)||(tailtype==1))		/
	{
		proto_tree_add_string(ColumnsTree, hf_dect_cc_TA, tvb, offset, 1, "[Ct]");

		if(tailtype==0)
			ep_strbuf_append_printf(afield_str,"C-Channel Next  Data: %s",tvb_bytes_to_str(tvb, offset, 5));
		else
			ep_strbuf_append_printf(afield_str,"C-Channel First Data: %s",tvb_bytes_to_str(tvb, offset, 5));

		proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);
	}
	else if((tailtype==2)||(tailtype==3))		/
	{
		proto_tree_add_string(ColumnsTree, hf_dect_cc_TA, tvb, offset, 1, "[Nt]");

		ep_strbuf_append_printf(afield_str,"RFPI: %s",tvb_bytes_to_str(tvb, offset, 5));
		proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);

		proto_tree_add_item(atailti, hf_dect_A_Tail_Nt, tvb, offset, 5, FALSE);
	}
	else if(tailtype==4)				/
	{
		proto_tree_add_string(ColumnsTree, hf_dect_cc_TA, tvb, offset, 1, "[Qt]");

		proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_Qh, tvb, offset, 1, FALSE);

		switch(tail_0>>4)
		{
		case 0:		/
		case 1:
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Static System Info");

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Nr, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Sn, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Sp, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Esc, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Txs, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Mc, tvb, offset, 1, FALSE);

			proto_tree_add_none_format(ATail, hf_dect_A_Tail_Qt_0_CA, tvb, offset, 2, " Carrier%s%s%s%s%s%s%s%s%s%s available",
				(tail_1&0x02)?" 0":"", (tail_1&0x01)?" 1":"", (tail_2&0x80)?" 2":"",
				(tail_2&0x40)?" 3":"", (tail_2&0x20)?" 4":"", (tail_2&0x10)?" 5":"",
				(tail_2&0x08)?" 6":"", (tail_2&0x04)?" 7":"", (tail_2&0x02)?" 8":"",
				(tail_2&0x01)?" 9":"");
			offset+=2;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Spr1, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Cn, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_Spr2, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_0_PSCN, tvb, offset, 1, FALSE);
			offset++;
			/
			offset-=5;
			break;
		case 2:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Extended RF Carriers Part 1");
			/
			break;
		case 3:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Fixed Part Capabilities");

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A12, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A13, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A14, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A15, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A16, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A17, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A18, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A19, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A20, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A21, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A22, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A23, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A24, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A25, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A26, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A27, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A28, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A29, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A30, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A31, tvb, offset, 1, FALSE);
			offset++;


			/
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A32, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A33, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A34, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A35, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A36, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A37, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A38, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A39, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A40, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A41, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A42, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A43, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A44, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A45, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A46, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_3_A47, tvb, offset, 1, FALSE);
			offset++;

			/
			offset-=5;
			break;
		case 4:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Extended Fixed Part Capabilities");


			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_CRFPHops, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_CRFPEnc, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_REFHops, tvb, offset, 2, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_REPCap, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_Sync, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A20, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_MACSusp, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_MACIpq, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A23, tvb, offset, 1, FALSE);
			offset++;


			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A24, tvb, offset, 1, FALSE);

			/
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A25, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A26, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A27, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A28, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A29, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A30, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A31, tvb, offset, 1, FALSE);
			offset++;


			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A32, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A33, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A34, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A35, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A36, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A37, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A38, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A39, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A40, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A41, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A42, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A43, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A44, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A45, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A46, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_4_A47, tvb, offset, 1, FALSE);
			offset++;

			/
			offset-=5;
			break;
		case 5:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "SARI List Contents");
			/
			break;
		case 6:		/
			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_6_Spare, tvb, offset, 2, FALSE);
			offset+=2;

			ep_strbuf_append_printf(afield_str,"Multi-Frame No.: %s",tvb_bytes_to_str(tvb, offset, 3));
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);

			proto_tree_add_item(ATail, hf_dect_A_Tail_Qt_6_Mfn, tvb, offset, 3, FALSE);
			offset+=3;

			/
			offset-=5;
			break;
		case 7:		/
			ep_strbuf_append_printf(afield_str,"Escape Data: %s",tvb_bytes_to_str(tvb, offset, 5));
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);
			break;
		case 8:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Obsolete");
			break;
		case 9:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Extended RF Carriers Part 2");
			/
			break;
		case 11:	/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Transmit Information");
			/
			break;
		case 12:	/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Extended Fixed Part Capabilities 2");
			/
			break;
		case 10:	/
		case 13:
		case 14:
		case 15:
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Reserved");
			break;
		}
	}
	else if(tailtype==5)				/
	{
	}
	else if((tailtype==6)||((tailtype==7)&&(dect_packet_type==DECT_PACKET_PP)))	/
	{
		proto_tree_add_string(ColumnsTree, hf_dect_cc_TA, tvb, offset, 1, "[Mt]");

		proto_tree_add_uint(ATail, hf_dect_A_Tail_Mt_Mh, tvb, offset, 1, tail_0);

		switch(tail_0>>4)
		{
		case 0:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Basic Connection Control");
			proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_BasicConCtrl, tvb, offset, 1, FALSE);
			offset++;

			if(((tail_0 & 0x0f)==6)||((tail_0 & 0x0f)==7))
			{
				/
				proto_tree_add_none_format(ATail, hf_dect_A_Tail_Mt_Mh_attr, tvb, offset, 4, "More infos at ETSI EN 300 175-3 V2.3.0  7.2.5.2.4");
				offset +=4;
			}
			else
			{
				proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Mh_fmid, tvb, offset, 2, FALSE);
				offset++;

				proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Mh_pmid, tvb, offset, 3, FALSE);
				offset+=3;
			}

			/
			offset-=5;
			break;
		case 1:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Advanced Connection Control");
			break;
		case 2:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "MAC Layer Test Messages");
			break;
		case 3:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Quality Control");
			break;
		case 4:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Broadcast and Connectionless Services");
			break;
		case 5:		/

			ep_strbuf_append_printf(afield_str,"Encryption Control: %s %s",
				val_to_str((tail_0&0x0c)>>2, MTEncrCmd1_vals, "Error, please report: %d"),
				val_to_str(tail_0&0x03, MTEncrCmd2_vals, "Error, please report: %d"));

			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);

			proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Encr_Cmd1, tvb, offset, 1, FALSE);
			proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Encr_Cmd2, tvb, offset, 1, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Mh_fmid, tvb, offset, 2, FALSE);
			offset++;

			proto_tree_add_item(ATail, hf_dect_A_Tail_Mt_Mh_pmid, tvb, offset, 3, FALSE);
			offset+=3;

			/
			offset-=5;
			break;
		case 6:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Tail for use with the first Transmission of a B-Field \"bearer request\" Message");
			break;
		case 7:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Escape");
			break;
		case 8:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "TARI Message");
			break;
		case 9:		/
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "REP Connection Control");
			break;
		case 10:	/
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, "Reserved");
			break;
		}
	}
	else if((tailtype==7)&&(dect_packet_type==DECT_PACKET_FP))	/
	{
		proto_tree_add_string(ColumnsTree, hf_dect_cc_TA, tvb, offset, 1, "[Pt]");

		proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_ExtFlag, tvb, offset, 1, FALSE);
		proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_SDU, tvb, offset, 1, FALSE);

		if(((tail_0&0x70)>>4)&0xfe)
			ep_strbuf_append_printf(afield_str,"%s, ",val_to_str((tail_0&0x70)>>4, PTSDU_vals, "Error, please report: %d"));
	
		switch((tail_0&0x70)>>4)
		{
		case 0:		/
		case 1:		/
			if(((tail_0&0x70)>>4)==0)
#if 0
XXX: Hier weitermachen
#endif
			{
				ep_strbuf_append_printf(afield_str,"RFPI: xxxxx%.1x%.2x%.2x, ", (pkt_afield->Tail[0]&0x0f), pkt_afield->Tail[1], pkt_afield->Tail[2]);
				proto_tree_add_none_format(atailti, hf_dect_A_Tail_Pt_RFPI, tvb, offset, 3, "RFPI: xxxxx%.1x%.2x%.2x", (pkt_afield->Tail[0]&0x0f), pkt_afield->Tail[1], pkt_afield->Tail[2]);
				offset+=3;

				proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_InfoType, tvb, offset, 1, FALSE);
			}
			else
			{
				ep_strbuf_append_printf(afield_str,"Bs Data: %.1x%.2x%.2x, ", (pkt_afield->Tail[0]&0x0f), pkt_afield->Tail[1], pkt_afield->Tail[2]);
				proto_tree_add_none_format(atailti, hf_dect_A_Tail_Pt_BsData, tvb, offset, 3, "Bs Data: %.1x%.2x%.2x", (pkt_afield->Tail[0]&0x0f), pkt_afield->Tail[1], pkt_afield->Tail[2]);
				offset+=3;

				proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_InfoType, tvb, offset, 1, FALSE);
			}

			ep_strbuf_append_printf(afield_str,"%s",val_to_str(pkt_afield->Tail[3]>>4, PTInfoType_vals, "Error, please report: %d"));

			switch(pkt_afield->Tail[3]>>4)
			{
			case 0: /
				proto_tree_add_none_format(ATail, hf_dect_A_Tail_Pt_Fillbits, tvb, offset, 2, "Fillbits: %.1x%.2x", pkt_afield->Tail[3]&0x0f, pkt_afield->Tail[4]);
				offset+=2;
				break;
			case 1: /
				offset+=2;
				break;
			case 7: /
				offset+=2;
				break;
			case 8: /
				proto_tree_add_none_format(ATail, hf_dect_A_Tail_Pt_SlotPairs, tvb, offset, 2, " Slot-Pairs: %s%s%s%s%s%s%s%s%s%s%s%s available",
					(pkt_afield->Tail[3]&0x08)?" 0/12":"", (pkt_afield->Tail[3]&0x04)?" 1/13":"", (pkt_afield->Tail[3]&0x02)?" 2/14":"",
					(pkt_afield->Tail[3]&0x01)?" 3/15":"", (pkt_afield->Tail[4]&0x80)?" 4/16":"", (pkt_afield->Tail[4]&0x40)?" 5/17":"",
					(pkt_afield->Tail[4]&0x20)?" 6/18":"", (pkt_afield->Tail[4]&0x10)?" 7/19":"", (pkt_afield->Tail[4]&0x08)?" 8/20":"",
					(pkt_afield->Tail[4]&0x04)?" 9/21":"", (pkt_afield->Tail[4]&0x02)?" 10/22":"", (pkt_afield->Tail[4]&0x01)?" 11/23":"");

				offset+=2;
				break;
			case 2: /
			case 3: /
			case 4: /
			case 5: /
			case 12: /
				proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_Bearer_Sn, tvb, offset, 1, FALSE);
				offset++;

				proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_Bearer_Sp, tvb, offset, 1, FALSE);
				proto_tree_add_item(ATail, hf_dect_A_Tail_Pt_Bearer_Cn, tvb, offset, 1, FALSE);
				offset++;
				break;
			case 6: /
				offset+=2;
				break;
			case 9: /
				offset+=2;
				break;
			case 10: /
				offset+=2;
				break;
			case 11: /
				offset+=2;
				break;
			case 13: /
				offset+=2;
				break;
			case 14: /
				offset+=2;
				break;
			case 15: /
				offset+=2;
				break;
			}
			/
			offset-=5;
			break;
		case 2:		/
			ep_strbuf_append_printf(afield_str,"Full Page");
			break;
		case 3:		/
			ep_strbuf_append_printf(afield_str,"MAC Resume Page");
			break;
		case 4:		/
			ep_strbuf_append_printf(afield_str,"Not the Last 36 Bits");
			break;
		case 5:		/
			ep_strbuf_append_printf(afield_str,"The First 36 Bits");
			break;
		case 6:		/
			ep_strbuf_append_printf(afield_str,"The Last 36 Bits");
			break;
		case 7:		/
			ep_strbuf_append_printf(afield_str,"All of a Long Page");
			break;
		}

		proto_tree_add_string(ColumnsTree, hf_dect_cc_AField, tvb, offset, 1, afield_str->str);
	}


	offset+=5;

	/

	memcpy(rcrcdat, pkt_ptr, 6);
	rcrcdat[6]=0;
	rcrcdat[7]=0;
	rcrc=calc_rcrc(rcrcdat);
	if(rcrc!=pkt_afield->RCRC)
		proto_tree_add_uint_format(afieldti, hf_dect_A_RCRC, tvb, offset, 2, 0, "R-CRC Error (Calc:%.4x, Recv:%.4x)", rcrc, pkt_afield->RCRC);
	else
		proto_tree_add_uint_format(afieldti, hf_dect_A_RCRC, tvb, offset, 2, 1, "R-CRC Match (Calc:%.4x, Recv:%.4x)", rcrc, pkt_afield->RCRC);

	offset+=2;

	/
	offset=dissect_bfield(dect_packet_type, header, pkt_bfield, pinfo, pkt_ptr, tvb, ti, DectTree, offset, ColumnsTree);
}
