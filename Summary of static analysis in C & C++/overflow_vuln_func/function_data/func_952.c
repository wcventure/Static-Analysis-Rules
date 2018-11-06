static void
dissect_ncp_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    gboolean is_tcp)
{
	proto_tree			*ncp_tree = NULL;
	proto_item			*ti;
	struct ncp_ip_header		ncpiph;
	struct ncp_ip_rqhdr		ncpiphrq;
	struct ncp_common_header	header;
	guint16				nw_connection, ncp_burst_seqno, ncp_ack_seqno;
	guint16				flags = 0;
	char				flags_str[1+3+1+3+1+3+1+1];
	char				*sep;
	proto_tree			*flags_tree = NULL;
	int				hdr_offset = 0;
	int				commhdr;
	int				offset = 0;
	gint				length_remaining;
	tvbuff_t       			*next_tvb;
	guint32				testvar = 0, ncp_burst_command, burst_len, burst_off, burst_file;
	guint8				subfunction;
	guint32				data_offset;
	guint16				data_len = 0;
	guint16				missing_fraglist_count = 0;
	mncp_rhash_value		*request_value = NULL;
	conversation_t			*conversation;

	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "NCP");
	if (check_col(pinfo->cinfo, COL_INFO))
		col_clear(pinfo->cinfo, COL_INFO);

	hdr_offset = 0;

	if (is_tcp) {
		if (tvb_get_ntohl(tvb, hdr_offset) != NCPIP_RQST && tvb_get_ntohl(tvb, hdr_offset) != NCPIP_RPLY) 
			hdr_offset += 1;
		ncpiph.signature	= tvb_get_ntohl(tvb, hdr_offset);
		ncpiph.length		= tvb_get_ntohl(tvb, hdr_offset+4);
		hdr_offset += 8;
		if (ncpiph.signature == NCPIP_RQST) {
			ncpiphrq.version	= tvb_get_ntohl(tvb, hdr_offset);
			hdr_offset += 4;
			ncpiphrq.rplybufsize	= tvb_get_ntohl(tvb, hdr_offset);
			hdr_offset += 4;
		}
		/
		conversation = find_conversation(pinfo->fd->num, &pinfo->src, &pinfo->dst,
		    PT_NCP, (guint32) pinfo->srcport, (guint32) pinfo->destport,
		    0);
		if ((ncpiph.length & 0x80000000) ||
		    ncpiph.signature == NCPIP_RPLY) {
			/
			if (!pinfo->fd->flags.visited) {
				if (conversation != NULL) {
					/
					request_value =
					    mncp_hash_lookup(conversation);
					/
					if (request_value == NULL) {
						request_value =
						    mncp_hash_insert(conversation);
					}
				} else {
					/
					conversation = conversation_new(pinfo->fd->num, &pinfo->src,
					    &pinfo->dst, PT_NCP,
					    (guint32) pinfo->srcport,
					    (guint32) pinfo->destport, 0);
					request_value =
					    mncp_hash_insert(conversation);
				}
				/
				if (ncpiph.signature == NCPIP_RQST) {
					hdr_offset += 8;
					ncpiph.length &= 0x7fffffff;
					request_value->packet_signature=TRUE;
				} else {
					/
					request_value =
					    mncp_hash_lookup(conversation);
					if (request_value->packet_signature) {
						hdr_offset += 8;
						ncpiph.length &= 0x7fffffff;
						/
						request_value->packet_signature=TRUE;
					} else {
						/
						request_value->packet_signature=FALSE;
					}
				}
			} else {
				/
				request_value = mncp_hash_lookup(conversation);
				if (request_value->packet_signature) {
					hdr_offset += 8;
					ncpiph.length &= 0x7fffffff;
				}
			}
		} else {
			if (!pinfo->fd->flags.visited) {
				if (conversation != NULL) {
					/
					request_value =
					    mncp_hash_lookup(conversation);
					/
					if (request_value == NULL) {
						request_value =
						    mncp_hash_insert(conversation);
					}
				} else {
					/
					conversation = conversation_new(pinfo->fd->num, &pinfo->src,
					    &pinfo->dst, PT_NCP,
					    (guint32) pinfo->srcport,
					    (guint32) pinfo->destport, 0);
					request_value =
					    mncp_hash_insert(conversation);
				}
				/
				request_value->packet_signature=FALSE;
			} else {
				request_value = mncp_hash_lookup(conversation);
			}
		}
	}

	/
	commhdr = hdr_offset;

	header.type		= tvb_get_ntohs(tvb, commhdr);
	header.sequence		= tvb_get_guint8(tvb, commhdr+2);
	header.conn_low		= tvb_get_guint8(tvb, commhdr+3);
	header.conn_high	= tvb_get_guint8(tvb, commhdr+5);

	if (check_col(pinfo->cinfo, COL_INFO)) {
	    col_add_fstr(pinfo->cinfo, COL_INFO,
		    "%s",
		    val_to_str(header.type, ncp_type_vals, "Unknown type (0x%04x)"));
	}

	nw_connection = (header.conn_high << 16) + header.conn_low;

	if (tree) {
		ti = proto_tree_add_item(tree, proto_ncp, tvb, 0, -1, FALSE);
		ncp_tree = proto_item_add_subtree(ti, ett_ncp);

		if (is_tcp) {
			proto_tree_add_uint(ncp_tree, hf_ncp_ip_sig, tvb, 0, 4, ncpiph.signature);
			proto_tree_add_uint(ncp_tree, hf_ncp_ip_length, tvb, 4, 4, ncpiph.length);
			if (ncpiph.signature == NCPIP_RQST) {
				proto_tree_add_uint(ncp_tree, hf_ncp_ip_ver, tvb, 8, 4, ncpiphrq.version);
				proto_tree_add_uint(ncp_tree, hf_ncp_ip_rplybufsize, tvb, 12, 4, ncpiphrq.rplybufsize);
				if (request_value->packet_signature==TRUE)
					proto_tree_add_item(ncp_tree, hf_ncp_ip_packetsig, tvb, 16, 8, FALSE);
			} else {
				if (request_value->packet_signature==TRUE)
					proto_tree_add_item(ncp_tree, hf_ncp_ip_packetsig, tvb, 8, 8, FALSE);
			}
		}
		proto_tree_add_uint(ncp_tree, hf_ncp_type,	tvb, commhdr + 0, 2, header.type);
	}


	/
	switch (header.type) {

	case NCP_BROADCAST_SLOT:    /
		proto_tree_add_uint(ncp_tree, hf_ncp_seq,	tvb, commhdr + 2, 1, header.sequence);
		proto_tree_add_uint(ncp_tree, hf_ncp_connection,tvb, commhdr + 3, 3, nw_connection);
		proto_tree_add_item(ncp_tree, hf_ncp_task,	tvb, commhdr + 4, 1, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_oplock_flag, tvb, commhdr + 9, 1, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_oplock_handle, tvb, commhdr + 10, 4, FALSE);
		break;

	case NCP_LIP_ECHO:    /
		proto_tree_add_item(ncp_tree, hf_lip_echo, tvb, commhdr, 13, FALSE);
		break;

	case NCP_BURST_MODE_XFER:	/
		/
		flags = tvb_get_guint8(tvb, commhdr + 2);
		strcpy(flags_str, "");
		sep = " (";
		if (flags & ABT) {
			strcat(flags_str, sep);
			strcat(flags_str, "ABT");
			sep = ",";
		}
		if (flags & BSY) {
			strcat(flags_str, sep);
			strcat(flags_str, "BSY");
			sep = ",";
		}
		if (flags & EOB) {
			strcat(flags_str, sep);
			strcat(flags_str, "EOB");
			sep = ",";
		}
		if (flags & LST) {
			strcat(flags_str, sep);
			strcat(flags_str, "LST");
			sep = ",";
		}
		if (flags & SYS) {
			strcat(flags_str, sep);
			strcat(flags_str, "SYS");
		}
		if (flags_str[0] != '\0')
			strcat(flags_str, ")");
		ti = proto_tree_add_uint_format(ncp_tree, hf_ncp_system_flags,
		    tvb, commhdr + 2, 1, flags, "Flags: 0x%04x%s", flags,
		    flags_str);
		flags_tree = proto_item_add_subtree(ti, ett_ncp_system_flags);
		proto_tree_add_item(flags_tree, hf_ncp_system_flags_abt,
		    tvb, commhdr + 2, 1, FALSE);
		proto_tree_add_item(flags_tree, hf_ncp_system_flags_bsy,
		    tvb, commhdr + 2, 1, FALSE);
		proto_tree_add_item(flags_tree, hf_ncp_system_flags_eob,
		    tvb, commhdr + 2, 1, FALSE);
		proto_tree_add_item(flags_tree, hf_ncp_system_flags_lst,
		    tvb, commhdr + 2, 1, FALSE);
		proto_tree_add_item(flags_tree, hf_ncp_system_flags_sys,
		    tvb, commhdr + 2, 1, FALSE);

		proto_tree_add_item(ncp_tree, hf_ncp_stream_type,
		    tvb, commhdr + 3, 1, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_src_connection,
		    tvb, commhdr + 4, 4, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_dst_connection,
		    tvb, commhdr + 8, 4, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_packet_seqno,
		    tvb, commhdr + 12, 4, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_delay_time,
		    tvb, commhdr + 16, 4, FALSE);
		ncp_burst_seqno = tvb_get_ntohs(tvb, commhdr+20);
		proto_tree_add_item(ncp_tree, hf_ncp_burst_seqno,
		    tvb, commhdr + 20, 2, FALSE);
		ncp_ack_seqno = tvb_get_ntohs(tvb, commhdr+22);
		proto_tree_add_item(ncp_tree, hf_ncp_ack_seqno,
		    tvb, commhdr + 22, 2, FALSE);
		proto_tree_add_item(ncp_tree, hf_ncp_burst_len,
		    tvb, commhdr + 24, 4, FALSE);
		data_offset = tvb_get_ntohl(tvb, commhdr + 28);
		proto_tree_add_uint(ncp_tree, hf_ncp_data_offset,
		    tvb, commhdr + 28, 4, data_offset);
		data_len = tvb_get_ntohs(tvb, commhdr + 32);
		proto_tree_add_uint(ncp_tree, hf_ncp_data_bytes,
		    tvb, commhdr + 32, 2, data_len);
		missing_fraglist_count = tvb_get_ntohs(tvb, commhdr + 34);
		proto_tree_add_item(ncp_tree, hf_ncp_missing_fraglist_count,
		    tvb, commhdr + 34, 2, FALSE);
		offset = commhdr + 36;
		if (!(flags & SYS) && ncp_burst_seqno == ncp_ack_seqno &&
		    data_offset == 0) {
			/
			if (data_len < 4)
				return;
			ncp_burst_command = tvb_get_ntohl(tvb, offset);
			proto_tree_add_item(ncp_tree, hf_ncp_burst_command,
			    tvb, offset, 4, FALSE);
			offset += 4;
			data_len -= 4;

			if (data_len < 4)
				return;
			burst_file = tvb_get_ntohl(tvb, offset);
			proto_tree_add_item(ncp_tree, hf_ncp_burst_file_handle,
			    tvb, offset, 4, FALSE);
			offset += 4;
			data_len -= 4;

			if (data_len < 8)
				return;
			proto_tree_add_item(ncp_tree, hf_ncp_burst_reserved,
			    tvb, offset, 8, FALSE);
			offset += 8;
			data_len -= 8;

			if (data_len < 4)
				return;
			burst_off = tvb_get_ntohl(tvb, offset);
			proto_tree_add_uint(ncp_tree, hf_ncp_burst_offset,
			    tvb, offset, 4, burst_off);
			offset += 4;
			data_len -= 4;

			if (data_len < 4)
				return;
			burst_len = tvb_get_ntohl(tvb, offset);
			proto_tree_add_uint(ncp_tree, hf_ncp_burst_len,
			    tvb, offset, 4, burst_len);
			offset += 4;
			data_len -= 4;

			if (check_col(pinfo->cinfo, COL_INFO)) {
				col_add_fstr(pinfo->cinfo, COL_INFO,
				    "%s %d bytes starting at offset %d in file 0x%08x",
				    val_to_str(ncp_burst_command,
				      burst_command, "Unknown (0x%08x)"),
				     burst_len, burst_off, burst_file);
			}
			break;
		} else {
			if (tvb_get_guint8(tvb, commhdr + 2) & 0x10) {
				if (check_col(pinfo->cinfo, COL_INFO)) {
					col_set_str(pinfo->cinfo, COL_INFO,
					    "End of Burst");
				}
			}
		}
		break;

	case NCP_ALLOCATE_SLOT:		/
		length_remaining = tvb_length_remaining(tvb, commhdr + 4);
		if (length_remaining > 4) { 
			testvar = tvb_get_ntohl(tvb, commhdr+4);
			if (testvar == 0x4c495020) {
				proto_tree_add_item(ncp_tree, hf_lip_echo, tvb, commhdr+4, 13, FALSE);
				break;
			}
		}
		/
    
	case NCP_POSITIVE_ACK:		/
	case NCP_SERVICE_REQUEST:	/
	case NCP_SERVICE_REPLY:		/
	case NCP_WATCHDOG:		/
	case NCP_DEALLOCATE_SLOT:	/
	default:
		proto_tree_add_uint(ncp_tree, hf_ncp_seq,	tvb, commhdr + 2, 1, header.sequence);
		proto_tree_add_uint(ncp_tree, hf_ncp_connection,tvb, commhdr + 3, 3, nw_connection);
		proto_tree_add_item(ncp_tree, hf_ncp_task,	tvb, commhdr + 4, 1, FALSE);
		break;
	}

	/
	switch (header.type) {

	case NCP_ALLOCATE_SLOT:		/
		length_remaining = tvb_length_remaining(tvb, commhdr + 4);
		if (length_remaining > 4) {
			testvar = tvb_get_ntohl(tvb, commhdr+4);
			if (testvar == 0x4c495020) {
				proto_tree_add_text(ncp_tree, tvb, commhdr, -1,
				    "Lip Echo Packet");
				/
			}
		}

	case NCP_SERVICE_REQUEST:	/
	case NCP_DEALLOCATE_SLOT:	/
	case NCP_BROADCAST_SLOT:	/
		next_tvb = tvb_new_subset(tvb, hdr_offset, -1, -1);
		if (tvb_get_guint8(tvb, commhdr+6) == 0x68) {
			subfunction = tvb_get_guint8(tvb, commhdr+7);
			switch (subfunction) {

			case 0x02:	/
				dissect_nds_request(next_tvb, pinfo,
				    nw_connection, header.sequence,
				    header.type, ncp_tree);
				break;

			case 0x01:	/
				dissect_ping_req(next_tvb, pinfo,
				    nw_connection, header.sequence,
				    header.type, ncp_tree);
				break;

			default:
				dissect_ncp_request(next_tvb, pinfo,
				    nw_connection, header.sequence,
				    header.type, ncp_tree);
				break;
			 }
		} else {
			dissect_ncp_request(next_tvb, pinfo, nw_connection,
			    header.sequence, header.type, ncp_tree);
		}
		break;

	case NCP_SERVICE_REPLY:		/
		next_tvb = tvb_new_subset(tvb, hdr_offset, -1, -1);
		nds_defrag(next_tvb, pinfo, nw_connection, header.sequence,
		    header.type, ncp_tree);
		break;

	case NCP_POSITIVE_ACK:		/
		/
		next_tvb = tvb_new_subset(tvb, hdr_offset, -1, -1);
		dissect_ncp_reply(next_tvb, pinfo, nw_connection,
		    header.sequence, header.type, ncp_tree);
		break;

	case NCP_WATCHDOG:		/
		/
		proto_tree_add_item(ncp_tree, hf_ncp_completion_code,
		    tvb, commhdr + 6, 1, TRUE);
		proto_tree_add_item(ncp_tree, hf_ncp_connection_status,
		    tvb, commhdr + 7, 1, TRUE);
		proto_tree_add_item(ncp_tree, hf_ncp_slot,
		    tvb, commhdr + 8, 1, TRUE);
		proto_tree_add_item(ncp_tree, hf_ncp_control_code,
		    tvb, commhdr + 9, 1, TRUE);
		/
		if (tvb_offset_exists(tvb, commhdr + 10)) {
			call_dissector(data_handle,
			    tvb_new_subset(tvb, commhdr + 10, -1, -1),
			    pinfo, ncp_tree);
		}
		break;

	case NCP_BURST_MODE_XFER:	/
		if (flags & SYS) {
			/
			while (missing_fraglist_count != 0) {
				proto_tree_add_item(ncp_tree, hf_ncp_missing_data_offset,
				    tvb, offset, 4, FALSE);
				offset += 4;
				proto_tree_add_item(ncp_tree, hf_ncp_missing_data_count,
				    tvb, offset, 2, FALSE);
				offset += 2;
				missing_fraglist_count--;
			}
		} else {
			/
			length_remaining = tvb_length_remaining(tvb, offset);
			if (length_remaining > data_len)
				length_remaining = data_len;
			if (data_len != 0) {
				call_dissector(data_handle,
				    tvb_new_subset(tvb, offset,
					length_remaining, data_len),
				    pinfo, ncp_tree);
			}
		}
		break;

	case NCP_LIP_ECHO:		/
		proto_tree_add_text(ncp_tree, tvb, commhdr, -1,
		    "Lip Echo Packet");
		break;

	default:
		if (tree) {
		    proto_tree_add_text(ncp_tree, tvb, commhdr + 6, -1,
			    "%s packets not supported yet",
			    val_to_str(header.type, ncp_type_vals,
				"Unknown type (0x%04x)"));
		}
		break;
 	}
}
