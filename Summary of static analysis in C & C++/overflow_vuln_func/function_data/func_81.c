static void _dissect_uasip(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, e_ua_direction direction)
{
	proto_item *uasip_item = NULL;
	proto_tree *uasip_tree = NULL;
	gint offset = 0;
	guint8 opcode = 0;

	if (noesip_enabled)
	{
		if (check_col(pinfo->cinfo, COL_PROTOCOL))
		{
			col_append_str(pinfo->cinfo, COL_PROTOCOL, "/NOE");
		}
	}
	else
	{
		if (check_col(pinfo->cinfo, COL_PROTOCOL))
		{
			col_append_str(pinfo->cinfo, COL_PROTOCOL, "/DL");
		}
	}

	opcode = tvb_get_guint8(tvb, offset);
	offset++;

	ua_tap_info.opcode = opcode;
	ua_tap_info.expseq = 0;
	ua_tap_info.sntseq = 0;

	if (check_col(pinfo->cinfo, COL_INFO))
	{
		col_add_fstr(pinfo->cinfo, COL_INFO, "%s", val_to_str(opcode, uasip_opcode_str, "unknown (0x%02x)"));
	}

	uasip_item = proto_tree_add_protocol_format(tree, proto_uasip, tvb, 0, 5, "SIP/NOE Protocol, %s", val_to_str(opcode, uasip_opcode_str, "unknown (0x%02x)"));
	uasip_tree = proto_item_add_subtree(uasip_item, ett_uasip);
	proto_tree_add_uint(uasip_tree, hf_uasip_opcode, tvb, 0, 1, opcode);

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
						rTLV(uasip_tree, &hf_uasip_version, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_WINDOW_SIZE:
						rTLV(uasip_tree, &hf_uasip_window_size, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_MTU:
						rTLV(uasip_tree, &hf_uasip_mtu, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_UDP_LOST:
						rTLV(uasip_tree, &hf_uasip_udp_lost, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_UDP_LOST_REINIT:
						rTLV(uasip_tree, &hf_uasip_udp_lost_reinit, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_KEEPALIVE:
						rTLV(uasip_tree, &hf_uasip_keepalive, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_QOS_IP_TOS:
						rTLV(uasip_tree, &hf_uasip_qos_ip_tos, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_QOS_8021_VLID:
						rTLV(uasip_tree, &hf_uasip_qos_8021_vlid, tvb, offset, L);
					break;
					
					case UAUDP_CONNECT_QOS_8021_PRI:
						rTLV(uasip_tree, &hf_uasip_qos_8021_pri, tvb, offset, L);
					break;
					
					default:
					break;
				}
				offset += (2 + L);
			}
		}
		break;

		case UAUDP_NACK:
		{
			proto_tree_add_uint(uasip_tree, hf_uasip_expseq, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
			ua_tap_info.expseq = tvb_get_ntohs(tvb, offset+0);
			/
			/
			
			if (noesip_enabled)
			{
				if (check_col(pinfo->cinfo, COL_INFO)) 
				{
					col_add_fstr(pinfo->cinfo, COL_INFO, "NACK");
				}
			}
			else
			{
				if (check_col(pinfo->cinfo, COL_INFO)) 
				{
					col_add_fstr(pinfo->cinfo, COL_INFO, "NACK exp:%d", ua_tap_info.expseq);
				}
			}
		}
		break;

		case UAUDP_DATA:
		{
			int datalen;

			proto_tree_add_uint(uasip_tree, hf_uasip_expseq, tvb, offset+0, 2, tvb_get_ntohs(tvb, offset+0));
			proto_tree_add_uint(uasip_tree, hf_uasip_sntseq, tvb, offset+2, 2, tvb_get_ntohs(tvb, offset+2));
			ua_tap_info.expseq = tvb_get_ntohs(tvb, offset+0);
			ua_tap_info.sntseq = tvb_get_ntohs(tvb, offset+2);
			offset += 4;
			datalen  = (tvb_length(tvb) - offset);

			if (noesip_enabled)
			{
				if (datalen > 0)
				{
					if (direction == SYS_TO_TERM)
					{
						call_dissector(ua_sys_to_term_handle, tvb_new_subset(tvb, offset, datalen, datalen), pinfo, tree);
					}
					else if (direction == TERM_TO_SYS)
					{
						call_dissector(ua_term_to_sys_handle, tvb_new_subset(tvb, offset, datalen, datalen), pinfo, tree);
					}
					else
					{
						if (check_col(pinfo->cinfo, COL_INFO))
						{
							col_add_str(pinfo->cinfo, COL_INFO, "DATA - Couldn't resolve direction.");
						}
					}
				}
				else
				{
					if (check_col(pinfo->cinfo, COL_INFO))
					{
						col_add_str(pinfo->cinfo, COL_INFO, "ACK");
					}
				}
			}
			else
			{
				if (datalen > 0)
				{
					if (check_col(pinfo->cinfo, COL_INFO))
					{
						col_add_fstr(pinfo->cinfo, COL_INFO, "DATA exp:%d", ua_tap_info.expseq);
						col_append_fstr(pinfo->cinfo, COL_INFO, " snt:%d", ua_tap_info.sntseq);
					}
				}
				else
				{
					if (check_col(pinfo->cinfo, COL_INFO))
					{
						col_add_fstr(pinfo->cinfo, COL_INFO, "ACK  exp:%d", ua_tap_info.expseq);
						col_append_fstr(pinfo->cinfo, COL_INFO, " snt:%d", ua_tap_info.sntseq);
					}
				}
			}
		}
		break;
		
		default:
		break;
	}
	tap_queue_packet(uasip_tap, pinfo, &ua_tap_info);
}
