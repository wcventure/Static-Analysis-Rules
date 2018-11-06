static void
dissect_gtp (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	struct _gtp_hdr	gtp_hdr;
	proto_tree	*gtp_tree, *flags_tree;
	proto_item	*ti, *tf;
	int		i, offset, length, gtp_prime, checked_field, mandatory;
	int		seq_no, flow_label;
	guint8		pdu_no, next_hdr = 0, ext_hdr_val;
	const guint8	*tid_val;
	gchar		*tid_str;
	guint32		teid;
	tvbuff_t	*next_tvb;
	guint8		sub_proto, acfield_len = 0, control_field;
	
	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "GTP");
	if (check_col(pinfo->cinfo, COL_INFO))
		col_clear(pinfo->cinfo, COL_INFO);
	
	tvb_memcpy(tvb, (guint8 *)&gtp_hdr, 0, 4);
	
	if (!(gtp_hdr.flags & 0x10))
		gtp_prime = 1;
	else
		gtp_prime = 0;
	
	switch ((gtp_hdr.flags >> 5) & 0x07) {
		case 0: 
			gtp_version = 0;
			break;
		case 1: 
			gtp_version = 1;
			break;
		default: 
			gtp_version = 1;
			break;
	}

	if (check_col(pinfo->cinfo, COL_INFO))
		col_add_str(pinfo->cinfo, COL_INFO, val_to_str(gtp_hdr.message, message_type, "Unknown"));
	
	if (tree) {
		ti = proto_tree_add_item (tree, proto_gtp, tvb, 0, -1, FALSE);
		gtp_tree = proto_item_add_subtree(ti, ett_gtp);
		
		tf = proto_tree_add_uint (gtp_tree, hf_gtp_flags, tvb, 0, 1, gtp_hdr.flags);
		flags_tree = proto_item_add_subtree (tf, ett_gtp_flags);
		
		proto_tree_add_uint (flags_tree, hf_gtp_flags_ver, tvb, 0, 1, gtp_hdr.flags);
		proto_tree_add_uint (flags_tree, hf_gtp_flags_pt, tvb, 0, 1, gtp_hdr.flags);
		
		switch (gtp_version) {
			case 0:
				proto_tree_add_uint (flags_tree, hf_gtp_flags_spare1, tvb, 0, 1, gtp_hdr.flags);
				proto_tree_add_boolean (flags_tree, hf_gtp_flags_snn, tvb, 0, 1, gtp_hdr.flags);
				break;
			case 1:
				proto_tree_add_uint (flags_tree, hf_gtp_flags_spare2, tvb, 0, 1, gtp_hdr.flags);
				proto_tree_add_boolean (flags_tree, hf_gtp_flags_e, tvb, 0, 1, gtp_hdr.flags);
				proto_tree_add_boolean (flags_tree, hf_gtp_flags_s, tvb, 0, 1, gtp_hdr.flags);
				proto_tree_add_boolean (flags_tree, hf_gtp_flags_pn, tvb, 0, 1, gtp_hdr.flags);
				break;
			default:
				break;
		}
				
		proto_tree_add_uint (gtp_tree, hf_gtp_message_type, tvb, 1, 1, gtp_hdr.message);
		
		gtp_hdr.length = g_ntohs (gtp_hdr.length);
		proto_tree_add_uint (gtp_tree, hf_gtp_length, tvb, 2, 2, gtp_hdr.length);
		
		offset = 4;
		
		if (gtp_prime) {
			seq_no = tvb_get_ntohs (tvb, offset);
			proto_tree_add_uint (gtp_tree, hf_gtp_seq_number, tvb, offset, 2, seq_no);
			offset += 2;
		} else
		switch (gtp_version) {
			case 0:
				seq_no = tvb_get_ntohs (tvb, offset);
				proto_tree_add_uint (gtp_tree, hf_gtp_seq_number, tvb, offset, 2, seq_no);
				offset += 2;
			
				flow_label = tvb_get_ntohs (tvb, offset);
				proto_tree_add_uint (gtp_tree, hf_gtp_flow_label, tvb, offset, 2, flow_label);
				offset += 2;
			
				pdu_no = tvb_get_guint8 (tvb, offset);
				proto_tree_add_uint (gtp_tree, hf_gtp_sndcp_number, tvb, offset, 1, pdu_no);
				offset += 4;
				
				tid_val = tvb_get_ptr(tvb, offset, 8);
				tid_str = id_to_str (tid_val);
				proto_tree_add_string (gtp_tree, hf_gtp_tid, tvb, offset, 8, tid_str);
				offset += 8;
				break;			
			case 1:
				teid = tvb_get_ntohl (tvb, offset);
				proto_tree_add_uint (gtp_tree, hf_gtp_teid, tvb, offset, 4, teid);
				offset += 4;

				if (gtp_hdr.flags & 0x07) {
					seq_no = tvb_get_ntohs (tvb, offset);
					proto_tree_add_uint (gtp_tree, hf_gtp_seq_number, tvb, offset, 2, seq_no);
					offset += 2;
					
					pdu_no = tvb_get_guint8 (tvb, offset);
					proto_tree_add_uint (gtp_tree, hf_gtp_npdu_number, tvb, offset, 1, pdu_no);
					offset++;
					
					next_hdr = tvb_get_guint8 (tvb, offset);
					proto_tree_add_uint (gtp_tree, hf_gtp_next, tvb, offset, 1, next_hdr);
					if (!next_hdr)
						offset++;
				}
				break;
			default:
				break;
		}
		
		
		if (gtp_hdr.message != GTP_MSG_TPDU) {
			proto_tree_add_text(gtp_tree, tvb, 0, 0, "[--- end of GTP header, beginning of extension headers ---]");
			length = tvb_length (tvb);
			mandatory = 0;		/
			for (;;) {
				if (offset >= length) 
					break;
				if (next_hdr) {
					ext_hdr_val = next_hdr;
					next_hdr = 0;
				}
				else
					ext_hdr_val = tvb_get_guint8 (tvb, offset);
				if (gtp_etsi_order) {
					checked_field = check_field_presence (gtp_hdr.message, ext_hdr_val , (int *)&mandatory);
					switch (checked_field) {
						case -2: proto_tree_add_text (gtp_tree, tvb, 0, 0, "[WARNING] message not found");
							 break;
						case -1: proto_tree_add_text (gtp_tree, tvb, 0, 0, "[WARNING] field not present");
							 break;
						case 0:  break;
						default: proto_tree_add_text (gtp_tree, tvb, offset, 1, "[WARNING] wrong next field, should be: %s", val_to_str(checked_field, gtp_val, "Unknown extension field"));
							 break;
					}
				}

				i = -1;
				while (gtpopt[++i].optcode) 
					if (gtpopt[i].optcode == ext_hdr_val) 
						break;
				offset = offset + (*gtpopt[i].decode)(tvb, offset, pinfo, gtp_tree);
			}
		}
	}
	
	if ((gtp_hdr.message == GTP_MSG_TPDU) && gtp_tpdu) {

		if (gtp_prime)
			offset = 6;
		else
		if (gtp_version == 1) {
			if (gtp_hdr.flags & 0x07)  {
				offset = 11;
				if (tvb_get_guint8 (tvb, offset) == 0)
					offset++;
			}
			else 
				offset = 8;
		}
		else
			offset = 20;

		sub_proto = tvb_get_guint8 (tvb, offset);

		if ((sub_proto >= 0x45) &&  (sub_proto <= 0x4e)) {
			/
			
			next_tvb = tvb_new_subset (tvb, offset, -1, -1);
			call_dissector(ip_handle, next_tvb, pinfo, tree);
			
		} else
                if ((sub_proto & 0xf0) == 0x60){
			/
			next_tvb = tvb_new_subset (tvb, offset, -1, -1);
			call_dissector (ipv6_handle, next_tvb, pinfo, tree);
		} else {
			/

			if (sub_proto == 0xff) {
				/
				control_field = tvb_get_guint8 (tvb, offset + 1);
				if (control_field == 0x03)
					/
					acfield_len = 2;
			}

			next_tvb = tvb_new_subset (tvb, offset + acfield_len, -1, -1);
			call_dissector (ppp_handle, next_tvb, pinfo, tree);
		}

		if (check_col(pinfo->cinfo, COL_PROTOCOL))
			col_append_str_gtp(pinfo->cinfo, COL_PROTOCOL, "GTP");
	}
}
