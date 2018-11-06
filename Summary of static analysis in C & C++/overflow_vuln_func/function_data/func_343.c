static void
dissect_bootp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_tree	*bp_tree;
	proto_item	*ti;
	proto_tree	*flag_tree;
	proto_item	*fi, *hidden_item;
	guint8		op;
	guint8		htype, hlen;
	const guint8	*haddr;
	int		voff, eoff, tmpvoff; /
	guint32		ip_addr;
	gboolean	at_end;
	const char	*dhcp_type = NULL;
	const guint8	*vendor_class_id = NULL;
	guint16		flags, secs;
	int		offset_delta;
	guint8		overload = 0; /

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "BOOTP");
	/
	col_clear(pinfo->cinfo, COL_INFO);

	op = tvb_get_guint8(tvb, 0);
	htype = tvb_get_guint8(tvb, 1);
	hlen = tvb_get_guint8(tvb, 2);
	switch (op) {

	case BOOTREQUEST:
		if ((htype == ARPHRD_ETHER || htype == ARPHRD_IEEE802)
		    && hlen == 6) {
			if (check_col(pinfo->cinfo, COL_INFO)) { 
				col_add_fstr(pinfo->cinfo, COL_INFO, "Boot Request from %s (%s)",
					     arphrdaddr_to_str(tvb_get_ptr(tvb, 28, hlen),
							       hlen, htype),
					     get_ether_name(tvb_get_ptr(tvb, 28, hlen)));
			}
		}
		else {
			if (check_col(pinfo->cinfo, COL_INFO)) {
				col_add_fstr(pinfo->cinfo, COL_INFO, "Boot Request from %s",
					     arphrdaddr_to_str(tvb_get_ptr(tvb, 28, hlen),
							       hlen, htype));
			}
		}
		break;

	case BOOTREPLY:
		col_set_str(pinfo->cinfo, COL_INFO, "Boot Reply");
		break;

	default:
		if (check_col(pinfo->cinfo, COL_INFO))  
			col_add_fstr(pinfo->cinfo, COL_INFO, "Unknown BOOTP message type (%u)", op);
		break;
	}

	voff = VENDOR_INFO_OFFSET;

	/
	if (tvb_bytes_exist(tvb, voff, 4) &&
	    (tvb_get_ntohl(tvb, voff) == 0x63825363)) {
		voff += 4;
	} else {
		voff += 64;
	}
	eoff = tvb_reported_length(tvb);

	/
	tmpvoff = voff;
	at_end = FALSE;
	while (tmpvoff < eoff && !at_end) {
		offset_delta = bootp_option(tvb, pinfo, 0, tmpvoff, eoff, TRUE, &at_end,
		    &dhcp_type, &vendor_class_id, &overload);
		if (offset_delta <= 0) {
			THROW(ReportedBoundsError);
		}
		tmpvoff += offset_delta;
	}

	/
	if (dhcp_type != NULL) {
		/
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "DHCP");
		if (check_col(pinfo->cinfo, COL_INFO))  
			col_add_fstr(pinfo->cinfo, COL_INFO, "DHCP %-8s - Transaction ID 0x%x",
				     dhcp_type, tvb_get_ntohl(tvb, 4));
		tap_queue_packet( bootp_dhcp_tap, pinfo, dhcp_type);
	}

	/

	ti = proto_tree_add_item(tree, proto_bootp, tvb, 0, -1, FALSE);
	bp_tree = proto_item_add_subtree(ti, ett_bootp);

	proto_tree_add_uint(bp_tree, hf_bootp_type, tvb,
				   0, 1,
				   op);
	proto_tree_add_uint_format_value(bp_tree, hf_bootp_hw_type, tvb,
					 1, 1,
					 htype,
					 "%s",
					 arphrdtype_to_str(htype,
						     "Unknown (0x%02x)"));
	proto_tree_add_uint(bp_tree, hf_bootp_hw_len, tvb,
			    2, 1, hlen);
	proto_tree_add_item(bp_tree, hf_bootp_hops, tvb,
			    3, 1, FALSE);
	proto_tree_add_item(bp_tree, hf_bootp_id, tvb,
			    4, 4, FALSE);
	/
	secs = tvb_get_letohs(tvb, 8);
	if (secs > 0 && secs <= 0xff) {
		ti = proto_tree_add_uint_format_value(bp_tree, hf_bootp_secs, tvb,
			    8, 2, secs, "%u", secs);
		expert_add_info_format(pinfo, ti, PI_MALFORMED, PI_NOTE,
			    "Seconds elapsed (%u) appears to be encoded as little-endian", secs);
	} else {
		proto_tree_add_item(bp_tree, hf_bootp_secs, tvb,
			    8, 2, FALSE);
	}
	flags = tvb_get_ntohs(tvb, 10);
	fi = proto_tree_add_uint(bp_tree, hf_bootp_flags, tvb,
			    10, 2, flags);
	proto_item_append_text(fi, " (%s)",
	    (flags & BOOTP_BC) ? "Broadcast" : "Unicast");
	flag_tree = proto_item_add_subtree(fi, ett_bootp_flags);
	proto_tree_add_boolean(flag_tree, hf_bootp_flags_broadcast, tvb,
			    10, 2, flags);
	proto_tree_add_uint(flag_tree, hf_bootp_flags_reserved, tvb,
			    10, 2, flags);
	proto_tree_add_item(bp_tree, hf_bootp_ip_client, tvb,
			    12, 4, FALSE);
	proto_tree_add_item(bp_tree, hf_bootp_ip_your, tvb,
			    16, 4, FALSE);
	proto_tree_add_item(bp_tree, hf_bootp_ip_server, tvb,
			    20, 4, FALSE);
	proto_tree_add_item(bp_tree, hf_bootp_ip_relay, tvb,
			    24, 4, FALSE);

	if (hlen > 0 && hlen <= 16) {
		haddr = tvb_get_ptr(tvb, 28, hlen);
		if ((htype == ARPHRD_ETHER || htype == ARPHRD_IEEE802)
		    && hlen == 6)
			proto_tree_add_item(bp_tree, hf_bootp_hw_ether_addr, tvb, 28, 6, FALSE);
		else
			/
			proto_tree_add_bytes_format_value(bp_tree, hf_bootp_hw_addr, tvb, 28, 16,
					   haddr,
					   "%s", arphrdaddr_to_str(haddr, hlen, htype));
		if ((16 - hlen) > 0)
			proto_tree_add_item(bp_tree, hf_bootp_hw_addr_padding, tvb, 28+hlen, 16-hlen, FALSE);
	} else {
		proto_tree_add_text(bp_tree,  tvb,
					   28, 16, "Client address not given");
	}

	if (overload & OPT_OVERLOAD_SNAME) {
		proto_tree_add_text (bp_tree, tvb,
			SERVER_NAME_OFFSET, SERVER_NAME_LEN,
			"Server name option overloaded by DHCP");
	} else {
		/
		if (tvb_get_guint8(tvb, SERVER_NAME_OFFSET) != '\0') {
			proto_tree_add_item(bp_tree, hf_bootp_server, tvb,
					   SERVER_NAME_OFFSET,
					   SERVER_NAME_LEN, FALSE);

		} else {
			proto_tree_add_string_format(bp_tree, hf_bootp_server, tvb,
						   SERVER_NAME_OFFSET,
						   SERVER_NAME_LEN,
						   "", "Server host name not given");
		}
	}

	if (overload & OPT_OVERLOAD_FILE) {
		proto_tree_add_text (bp_tree, tvb,
			FILE_NAME_OFFSET, FILE_NAME_LEN,
			"Boot file name option overloaded by DHCP");
	} else {
		/
		if (tvb_get_guint8(tvb, FILE_NAME_OFFSET) != '\0') {
			proto_tree_add_item(bp_tree, hf_bootp_file, tvb,
					   FILE_NAME_OFFSET,
					   FILE_NAME_LEN, FALSE);
		} else {
			proto_tree_add_string_format(bp_tree, hf_bootp_file, tvb,
						   FILE_NAME_OFFSET,
						   FILE_NAME_LEN,
						   "", "Boot file name not given");
		}
	}

	voff = VENDOR_INFO_OFFSET;
	if (dhcp_type != NULL) {
		hidden_item = proto_tree_add_boolean(bp_tree, hf_bootp_dhcp, tvb, 0, 0, 1);
		PROTO_ITEM_SET_HIDDEN(hidden_item);
	}
	if (tvb_bytes_exist(tvb, voff, 4) &&
	    (tvb_get_ntohl(tvb, voff) == 0x63825363)) {
		ip_addr = tvb_get_ipv4(tvb, voff);
		proto_tree_add_ipv4_format_value(bp_tree, hf_bootp_cookie, tvb,
			voff, 4, ip_addr, "DHCP");
		voff += 4;
	} else {
		proto_tree_add_text(bp_tree,  tvb,
			voff, 64, "Bootp vendor specific options");
		voff += 64;
	}

	at_end = FALSE;
	while (voff < eoff && !at_end) {
		offset_delta = bootp_option(tvb, pinfo, bp_tree, voff, eoff, FALSE, &at_end,
		    &dhcp_type, &vendor_class_id, &overload);
		if (offset_delta <= 0) {
			THROW(ReportedBoundsError);
		}
		voff += offset_delta;
	}
	if ((dhcp_type != NULL) && (!at_end))
	{
		expert_add_info_format(pinfo, ti, PI_PROTOCOL, PI_ERROR, "End option missing");
	}
	if (voff < eoff) {
		/
		proto_tree_add_text(bp_tree, tvb, voff, eoff - voff, "Padding");
	}
}
