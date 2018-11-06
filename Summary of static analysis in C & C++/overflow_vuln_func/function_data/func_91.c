static void _dissect_uaudp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
						   e_ua_direction direction)
{
	proto_item *uaudp_item = NULL;
	proto_tree *uaudp_tree = NULL;
	gint offset = 0;
	guint8 opcode = 0;

	/
	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "UAUDP");

	/
	opcode = tvb_get_guint8(tvb, offset);
	offset++;

	ua_tap_info.opcode = opcode;
	ua_tap_info.expseq = 0;
	ua_tap_info.sntseq = 0;

	/

	if (check_col(pinfo->cinfo, COL_INFO))
		col_add_fstr(pinfo->cinfo,
			 COL_INFO,
			 "%s",
			 val_to_str(opcode, uaudp_opcode_str, "unknown (0x%02x)"));

	if (tree)
	{
		uaudp_item = proto_tree_add_protocol_format(tree, proto_uaudp, tvb, 0, 5,
			"Universal Alcatel/UDP Encapsulation Protocol, %s",
			val_to_str(opcode, uaudp_opcode_str, "unknown (0x%02x)"));

		uaudp_tree = proto_item_add_subtree(uaudp_item, ett_uaudp);

		/
		proto_tree_add_uint(uaudp_tree, hf_uaudp_opcode, tvb, 0, 1, opcode);

		switch(opcode)
		{
		case UAUDP_CONNECT:
		{
			while(tvb_offset_exists(tvb, offset))
			{
				guint8 T = tvb_get_guint8(tvb, offset+0);
				guint8 L = tvb_get_guint8(tvb, offset+1);

				switch(T)
				{
				case UAUDP_CONNECT_VERSION:
					rV(uaudp_tree, &hf_uaudp_version        , tvb, offset, L);
					break;
				case UAUDP_CONNECT_WINDOW_SIZE:
					rV(uaudp_tree, &hf_uaudp_window_size    , tvb, offset, L);
					break;
				case UAUDP_CONNECT_MTU:
					rV(uaudp_tree, &hf_uaudp_mtu            , tvb, offset, L);
					break;
				case UAUDP_CONNECT_UDP_LOST:
					rV(uaudp_tree, &hf_uaudp_udp_lost       , tvb, offset, L);
					break;
				case UAUDP_CONNECT_UDP_LOST_REINIT:
					rV(uaudp_tree, &hf_uaudp_udp_lost_reinit, tvb, offset, L);
					break;
				case UAUDP_CONNECT_KEEPALIVE:
					rV(uaudp_tree, &hf_uaudp_keepalive      , tvb, offset, L);
					break;
				case UAUDP_CONNECT_QOS_IP_TOS:
					rV(uaudp_tree, &hf_uaudp_qos_ip_tos     , tvb, offset, L);
					break;
				case UAUDP_CONNECT_QOS_8021_VLID:
					rV(uaudp_tree, &hf_uaudp_qos_8021_vlid  , tvb, offset, L);
					break;
				case UAUDP_CONNECT_QOS_8021_PRI:
					rV(uaudp_tree, &hf_uaudp_qos_8021_pri   , tvb, offset, L);
					break;
				}
				offset += (2 + L);
			}
			break;
		}

		case UAUDP_NACK:
		{
			proto_tree_add_uint(uaudp_tree,
								hf_uaudp_expseq,
								tvb,
								offset,
								2,
								tvb_get_ntohs(tvb, offset));
			/
			/ 
			break;
		}

		case UAUDP_DATA:
		{
			int datalen;

			proto_tree_add_uint(uaudp_tree,
								hf_uaudp_expseq,
								tvb,
								offset+0,
								2,
								tvb_get_ntohs(tvb, offset+0));

			proto_tree_add_uint(uaudp_tree,
								hf_uaudp_sntseq,
								tvb,
								offset+2,
								2,
								tvb_get_ntohs(tvb, offset+2));

			ua_tap_info.expseq = hf_uaudp_expseq;
			ua_tap_info.sntseq = hf_uaudp_sntseq;
			offset  += 4;
			datalen  = (tvb_length(tvb) - offset);

			/
			if (datalen > 0)
			{
				if (direction==SYS_TO_TERM)
					call_dissector(ua_sys_to_term_handle,
								   tvb_new_subset(tvb, offset, datalen, datalen),
								   pinfo,
								   tree);
				else if (direction==TERM_TO_SYS)
					call_dissector(ua_term_to_sys_handle,
								   tvb_new_subset(tvb, offset, datalen, datalen),
								   pinfo,
								   tree);
				else {
					if (check_col(pinfo->cinfo, COL_INFO))
						col_add_str(pinfo->cinfo,
							 COL_INFO,
							 "Data - Couldn't resolve direction. Check UAUDP Preferences.");
				}
				ua_tap_info.expseq = hf_uaudp_expseq;
			}
			else {
				/
				if (check_col(pinfo->cinfo, COL_INFO))
					col_add_str(pinfo->cinfo,
								COL_INFO,
								"Data ACK");
			}
			break;
		}
		default:
			break;
		}
	}
	tap_queue_packet(uaudp_tap, pinfo, &ua_tap_info);
}
