static int
bootp_option(tvbuff_t *tvb, packet_info *pinfo, proto_tree *bp_tree, int voff,
    int eoff, gboolean first_pass, gboolean *at_end, const char **dhcp_type_p,
    const guint8 **vendor_class_id_p, guint8 *overload_p)
{
	const char		*text;
	enum field_type		ftype;
	guchar			code = tvb_get_guint8(tvb, voff);
	int			optlen;
	const struct true_false_string *tfs;
	const value_string	*vs;
	guchar			byte;
	int			i, consumed;
	int			optoff, optleft, optend;
	guint32			time_u_secs;
	gint32			time_s_secs;
	proto_tree		*v_tree, *ft;
	proto_item		*vti;
	guint8			protocol;
	guint8			algorithm;
	guint8			rdm;
	guint8			fqdn_flags;
	int			o52voff, o52eoff;
	gboolean		o52at_end;
	guint8			s_option;
	guint8			s_len;
	const guchar		*dns_name;


	static const value_string slpda_vals[] = {
		{0x00,   "Dynamic Discovery" },
		{0x01,   "Static Discovery" },
		{0x80,   "Backwards compatibility" },
		{0,     NULL     } };

	static const value_string slp_scope_vals[] = {
		{0x00,   "Preferred Scope" },
		{0x01,   "Mandatory Scope" },
		{0,     NULL     } };

	static const value_string authen_protocol_vals[] = {
		{AUTHEN_PROTO_CONFIG_TOKEN,   "configuration token" },
		{AUTHEN_PROTO_DELAYED_AUTHEN, "delayed authentication" },
		{0,                           NULL     } };

	static const value_string authen_da_algo_vals[] = {
		{AUTHEN_DELAYED_ALGO_HMAC_MD5, "HMAC_MD5" },
		{0,                            NULL     } };

	static const value_string authen_rdm_vals[] = {
		{AUTHEN_RDM_MONOTONIC_COUNTER, "Monotonically-increasing counter" },
		{0,                            NULL     } };

	static const value_string opt_overload_vals[] = {
		{ OPT_OVERLOAD_FILE,  "Boot file name holds options",                },
		{ OPT_OVERLOAD_SNAME, "Server host name holds options",              },
		{ OPT_OVERLOAD_BOTH,  "Boot file and server host names hold options" },
		{ 0,                  NULL                                           } };

	/
	switch (code) {

	case 0:		/
		/
		for (i = voff + 1; i < eoff; i++ ) {
			if (tvb_get_guint8(tvb, i) != 0) {
				break;
			}
		}
		i = i - voff;
		if (!first_pass) {
			if (bp_tree != NULL) {
				proto_tree_add_text(bp_tree, tvb, voff, i,
				    "Padding (%d byte%s)", i, plurality(i, "", "s"));
			}
		}
		consumed = i;
		return consumed;

	case 255:	/
		if (!first_pass) {
			if (bp_tree != NULL) {
				proto_tree_add_text(bp_tree, tvb, voff, 1,
				    "End Option%s", *overload_p?" (overload)":"");
			}
		}
		*at_end = TRUE;
		consumed = 1;
		return consumed;
	}

	/
	if (first_pass) {
		if (!tvb_bytes_exist(tvb, voff+1, 1)) {
			/
			return 1;
		}
	}
	optlen = tvb_get_guint8(tvb, voff+1);
	consumed = optlen + 2;

	/
	if (first_pass) {
		if (tvb_bytes_exist(tvb, voff+2, consumed-2)) {
			switch (code) {

			case 52:
				*overload_p = tvb_get_guint8(tvb, voff+2);
				break;

			case 53:
				*dhcp_type_p =
				    val_to_str(tvb_get_guint8(tvb, voff+2),
					opt53_text,
					"Unknown Message Type (0x%02x)");
				break;

			case 60:
				*vendor_class_id_p =
				    tvb_get_ptr(tvb, voff+2, consumed-2);
				break;
			}
		}

		/
		return consumed;
	}

	/
	text = bootp_get_opt_text(code);
	ftype = bootp_get_opt_ftype(code);

	optoff = voff+2;

	vti = proto_tree_add_text(bp_tree, tvb, voff, consumed,
	    "Option: (t=%d,l=%d) %s", code, optlen, text);
	v_tree = proto_item_add_subtree(vti, ett_bootp_option);
	proto_tree_add_uint_format_value(v_tree, hf_bootp_option_type,
		tvb, voff, 1, code, "(%d) %s", code, text);
	proto_tree_add_item(v_tree, hf_bootp_option_length, tvb, voff+1, 1, FALSE);
	if (optlen > 0) {
		proto_tree_add_item(v_tree, hf_bootp_option_value, tvb, voff+2, optlen, FALSE);
	}

	/
	switch (code) {

	case 21:	/
		if (optlen == 8) {
			/
			proto_item_append_text(vti, " = %s/%s",
				ip_to_str(tvb_get_ptr(tvb, optoff, 4)),
				ip_to_str(tvb_get_ptr(tvb, optoff+4, 4)));
		} else {
			/
			for (i = optoff, optleft = optlen;
			    optleft > 0; i += 8, optleft -= 8) {
				if (optleft < 8) {
					proto_tree_add_text(v_tree, tvb, i, optleft,
					    "Option length isn't a multiple of 8");
					break;
				}
				proto_tree_add_text(v_tree, tvb, i, 8, "IP Address/Mask: %s/%s",
					ip_to_str(tvb_get_ptr(tvb, i, 4)),
					ip_to_str(tvb_get_ptr(tvb, i+4, 4)));
			}
		}
		break;

	case 33:	/
		if (optlen == 8) {
			/
			proto_item_append_text(vti, " = %s/%s",
				ip_to_str(tvb_get_ptr(tvb, optoff, 4)),
				ip_to_str(tvb_get_ptr(tvb, optoff+4, 4)));
		} else {
			/
			for (i = optoff, optleft = optlen; optleft > 0;
			    i += 8, optleft -= 8) {
				if (optleft < 8) {
					proto_tree_add_text(v_tree, tvb, i, optleft,
					    "Option length isn't a multiple of 8");
					break;
				}
				proto_tree_add_text(v_tree, tvb, i, 8,
					"Destination IP Address/Router: %s/%s",
					ip_to_str(tvb_get_ptr(tvb, i, 4)),
					ip_to_str(tvb_get_ptr(tvb, i+4, 4)));
			}
		}
		break;

	case 43:	/
		s_option = tvb_get_guint8(tvb, optoff);

		/
		if (*vendor_class_id_p != NULL &&
		    strncmp((const gchar*)*vendor_class_id_p, "PXEClient", strlen("PXEClient")) == 0) {
			proto_item_append_text(vti, " (PXEClient)");
			v_tree = proto_item_add_subtree(vti, ett_bootp_option);

			optend = optoff + optlen;
			while (optoff < optend) {
				optoff = dissect_vendor_pxeclient_suboption(v_tree,
					tvb, optoff, optend);
			}
		} else if (*vendor_class_id_p != NULL &&
			   ((strncmp((const gchar*)*vendor_class_id_p, "pktc", strlen("pktc")) == 0) ||
			    (strncmp((const gchar*)*vendor_class_id_p, "docsis", strlen("docsis")) == 0) ||
			    (strncmp((const gchar*)*vendor_class_id_p, "OpenCable2.0", strlen("OpenCable2.0")) == 0) ||
			    (strncmp((const gchar*)*vendor_class_id_p, "CableHome", strlen("CableHome")) == 0))) {
			/
			proto_item_append_text(vti, " (CableLabs)");

			optend = optoff + optlen;
			while (optoff < optend) {
				optoff = dissect_vendor_cablelabs_suboption(v_tree,
					tvb, optoff, optend);
			}
		} else if (s_option==58 || s_option==64 || s_option==65
			|| s_option==66 || s_option==67) {
			/
			/
			/
			/
			proto_item_append_text(vti, " (Alcatel-Lucent)");
			optend = optoff + optlen;
			while (optoff < optend) {
				optoff = dissect_vendor_alcatel_suboption(v_tree,
					tvb, optoff, optend);
			}
		}
		break;

	case 52:	/
		if (optlen < 1) {
			proto_item_append_text(vti, " length isn't >= 1");
			break;
		}
		byte = tvb_get_guint8(tvb, optoff);
		proto_item_append_text(vti, " = %s",
			val_to_str(byte, opt_overload_vals,
			    "Unknown (0x%02x)"));

		/
		if (voff > VENDOR_INFO_OFFSET && byte >= 1 && byte <= 3) {
			if (byte & OPT_OVERLOAD_FILE) {
				proto_item *oti;
				oti = proto_tree_add_text (bp_tree, tvb,
					FILE_NAME_OFFSET, FILE_NAME_LEN,
					"Boot file name option overload");
				o52voff = FILE_NAME_OFFSET;
				o52eoff = FILE_NAME_OFFSET + FILE_NAME_LEN;
				o52at_end = FALSE;
				while (o52voff < o52eoff && !o52at_end) {
					o52voff += bootp_option(tvb, pinfo, bp_tree, o52voff,
						o52eoff, FALSE, &o52at_end,
						dhcp_type_p, vendor_class_id_p,
						overload_p);
				}
				if (!o52at_end)
				{
					expert_add_info_format(pinfo, oti, PI_PROTOCOL,
						PI_ERROR, "file overload end option missing");
				}
			}
			if (byte & OPT_OVERLOAD_SNAME) {
				proto_item *oti;
				oti = proto_tree_add_text (bp_tree, tvb,
					SERVER_NAME_OFFSET, SERVER_NAME_LEN,
					"Server host name option overload");
				o52voff = SERVER_NAME_OFFSET;
				o52eoff = SERVER_NAME_OFFSET + SERVER_NAME_LEN;
				o52at_end = FALSE;
				while (o52voff < o52eoff && !o52at_end) {
					o52voff += bootp_option(tvb, pinfo, bp_tree, o52voff,
						o52eoff, FALSE, &o52at_end,
						dhcp_type_p, vendor_class_id_p,
						overload_p);
				}
				if (!o52at_end)
				{
					expert_add_info_format(pinfo, oti, PI_PROTOCOL,
						PI_ERROR, "sname overload end option missing");
				}
			}
			/
			*overload_p = 0;
		}
		break;

	case 53:	/
		if (optlen != 1) {
			proto_item_append_text(vti, " length isn't 1");
			break;
		}
		proto_item_append_text(vti, " = DHCP %s",
			val_to_str(tvb_get_guint8(tvb, optoff),
				opt53_text,
				"Unknown Message Type (0x%02x)"));
		break;

	case 55:	/
		for (i = 0; i < optlen; i++) {
			byte = tvb_get_guint8(tvb, optoff+i);
			proto_tree_add_text(v_tree, tvb, optoff+i, 1, "%d = %s",
					byte, bootp_get_opt_text(byte));
		}
		break;

	case 60:	/
		/
		proto_item_append_text(vti, " = \"%s\"",
			tvb_format_stringzpad(tvb, optoff, consumed-2));
		if ((tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_MTA_CAP10,
				      (int)strlen(PACKETCABLE_MTA_CAP10)) == 0)
		    ||
		    (tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_MTA_CAP15,
				      (int)strlen(PACKETCABLE_MTA_CAP15)) == 0)
			||
			(tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_MTA_CAP20,
				      (int)strlen(PACKETCABLE_MTA_CAP20)) == 0))
		{
			dissect_packetcable_mta_cap(v_tree, tvb, optoff, optlen);
		} else 
			if ((tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_CM_CAP11,
				(int)strlen(PACKETCABLE_CM_CAP11)) == 0)
			||
			(tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_CM_CAP20,
				(int)strlen(PACKETCABLE_CM_CAP20)) == 0 ))
		{
			dissect_docsis_cm_cap(v_tree, tvb, optoff, optlen);
		} else
			if (tvb_memeql(tvb, optoff, (const guint8*)PACKETCABLE_CM_CAP30,
				(int)strlen(PACKETCABLE_CM_CAP30)) == 0 )
		{
			proto_tree_add_text(v_tree, tvb, optoff, optlen,
				"vendor-class-data: \"%s\"", tvb_format_stringzpad(tvb, optoff, optlen));
		}
		break;

	case 61:	/
		if (optlen > 0)
			byte = tvb_get_guint8(tvb, optoff);
		else
			byte = 0;

		/
		/
		/

		if (optlen == 7 && byte > 0 && byte < 48) {
			proto_tree_add_text(v_tree, tvb, optoff, 1,
				"Hardware type: %s",
				arphrdtype_to_str(byte,
					"Unknown (0x%02x)"));
			if (byte == ARPHRD_ETHER || byte == ARPHRD_IEEE802)
				proto_tree_add_item(v_tree,
				    hf_bootp_hw_ether_addr, tvb, optoff+1, 6,
				    FALSE);
			else
				proto_tree_add_text(v_tree, tvb, optoff+1, 6,
					"Client hardware address: %s",
					arphrdaddr_to_str(tvb_get_ptr(tvb, optoff+1, 6),
					6, byte));
		} else if (optlen == 17 && byte == 0) {
			/
			proto_tree_add_item(v_tree, hf_bootp_client_identifier_uuid,
					    tvb, optoff + 1, 16, TRUE);
		/
		} else if (byte == 255) {
			guint16	duidtype;
			guint16	hwtype;
			guint8	*buf;
			int	enterprise; 

			/
			proto_tree_add_text(v_tree, tvb, optoff+1, 4,
				"IAID: %s",
				arphrdaddr_to_str(tvb_get_ptr(tvb, optoff+1, 4),
				4, byte));
			optoff = optoff + 5;
			duidtype = tvb_get_ntohs(tvb, optoff);
			proto_tree_add_text(v_tree, tvb, optoff, 2,
				"DUID type: %s (%u)",
						val_to_str(duidtype,
							   duidtype_vals, "Unknown"),
						duidtype);
			switch (duidtype) {
			case DUID_LLT:
				if (optlen < 8) {
					proto_tree_add_text(v_tree, tvb, optoff,
						optlen, "DUID: malformed option");
					break;
				}
				hwtype=tvb_get_ntohs(tvb, optoff + 2);
				proto_tree_add_text(v_tree, tvb, optoff + 2, 2,
					"Hardware type: %s (%u)", arphrdtype_to_str(hwtype, "Unknown"),
					hwtype);
				/
				proto_tree_add_text(v_tree, tvb, optoff + 4, 4,
					"Time: %u", tvb_get_ntohl(tvb, optoff + 4));
				if (optlen > 8) {
					proto_tree_add_text(v_tree, tvb, optoff + 8,
						optlen - 13, "Link-layer address: %s",
						arphrdaddr_to_str(tvb_get_ptr(tvb, optoff+8, optlen-13), optlen-13, hwtype));
				}
				break;
			case DUID_EN:
				if (optlen < 6) {
					proto_tree_add_text(v_tree, tvb, optoff,
						optlen, "DUID: malformed option");
					break;
				}
				enterprise = tvb_get_ntohl(tvb, optoff+2);
				proto_tree_add_text(v_tree, tvb, optoff + 2, 4,
					    "Enterprise-number: %s (%u)",
					    val_to_str( enterprise, sminmpec_values, "Unknown"),
					    enterprise);
				if (optlen > 6) {
						buf = tvb_bytes_to_str(tvb, optoff + 6, optlen - 11);
					proto_tree_add_text(v_tree, tvb, optoff + 6,
						optlen - 11, "identifier: %s", buf);
				}
				break;
			case DUID_LL:
				if (optlen < 4) {
					proto_tree_add_text(v_tree, tvb, optoff,
						optlen, "DUID: malformed option");
					break;
				}
				hwtype=tvb_get_ntohs(tvb, optoff + 2);
				proto_tree_add_text(v_tree, tvb, optoff + 2, 2,
					"Hardware type: %s (%u)",
					arphrdtype_to_str(hwtype, "Unknown"),
					hwtype);
				if (optlen > 4) {
					proto_tree_add_text(v_tree, tvb, optoff + 4,
						optlen - 9, "Link-layer address: %s",
						arphrdaddr_to_str(tvb_get_ptr(tvb, optoff+4, optlen-9), optlen-9, hwtype));
				}
				break;
			}
		} else {
			/
		}
		break;

	case 97:        /
		if (optlen > 0)
			byte = tvb_get_guint8(tvb, optoff);
		else
			byte = 0;

		/
		/
		/

		if (optlen == 7 && byte > 0 && byte < 48) {
			proto_tree_add_text(v_tree, tvb, optoff, 1,
				"Hardware type: %s",
				arphrdtype_to_str(byte,
					"Unknown (0x%02x)"));
			if (byte == ARPHRD_ETHER || byte == ARPHRD_IEEE802)
				proto_tree_add_item(v_tree,
				    hf_bootp_hw_ether_addr, tvb, optoff+1, 6,
				    FALSE);
			else
				proto_tree_add_text(v_tree, tvb, optoff+1, 6,
					"Client hardware address: %s",
					arphrdaddr_to_str(tvb_get_ptr(tvb, optoff+1, 6),
					6, byte));
		} else if (optlen == 17 && byte == 0) {
			/
			proto_tree_add_item(v_tree, hf_bootp_client_identifier_uuid,
					    tvb, optoff + 1, 16, TRUE);
		} else {
			/
		}
		break;

	case 63:	/

		optend = optoff + optlen;
		while (optoff < optend)
			optoff = dissect_netware_ip_suboption(v_tree, tvb, optoff, optend);
		break;

	case 78:	/
		if (optlen < 1) {
			proto_item_append_text(vti, " length isn't >= 1");
			break;
		}
		optleft = optlen;
		byte = tvb_get_guint8(tvb, optoff);
		proto_item_append_text(vti, " = %s",
				val_to_str(byte, slpda_vals,
					"Unknown (0x%02x)"));
		optoff++;
		optleft--;
		if (byte == 0x80) {
			if (optleft == 0)
				break;
			optoff++;
			optleft--;
		}
		for (i = optoff; optleft > 0; i += 4, optleft -= 4) {
			if (optleft < 4) {
				proto_tree_add_text(v_tree, tvb, i, optleft,
				    "Option length isn't a multiple of 4");
				break;
			}
			proto_tree_add_text(v_tree, tvb, i, 4, "SLPDA Address: %s",
			    ip_to_str(tvb_get_ptr(tvb, i, 4)));
		}
		break;

	case 79:	/
		byte = tvb_get_guint8(tvb, optoff);
		proto_item_append_text(vti, " = %s",
				val_to_str(byte, slp_scope_vals,
				    "Unknown (0x%02x)"));
		optoff++;
		optleft = optlen - 1;
		proto_tree_add_text(v_tree, tvb, optoff, optleft,
		    "%s = \"%s\"", text,
		    tvb_format_stringzpad(tvb, optoff, optleft));
		break;

	case 81:	/
		if (optlen < 3) {
			proto_item_append_text(vti, " length isn't >= 3");
			break;
		}
		fqdn_flags = tvb_get_guint8(tvb, optoff);
		ft = proto_tree_add_text(v_tree, tvb, optoff, 1, "Flags: 0x%02x", fqdn_flags);
		proto_tree_add_item(v_tree, hf_bootp_fqdn_mbz, tvb, optoff, 1, FALSE);
		proto_tree_add_item(v_tree, hf_bootp_fqdn_n, tvb, optoff, 1, FALSE);
		proto_tree_add_item(v_tree, hf_bootp_fqdn_e, tvb, optoff, 1, FALSE);
		proto_tree_add_item(v_tree, hf_bootp_fqdn_o, tvb, optoff, 1, FALSE);
		proto_tree_add_item(v_tree, hf_bootp_fqdn_s, tvb, optoff, 1, FALSE);
		/
		proto_tree_add_item(v_tree, hf_bootp_fqdn_rcode1, tvb, optoff+1, 1, FALSE);
		/
		proto_tree_add_item(v_tree, hf_bootp_fqdn_rcode2, tvb, optoff+2, 1, FALSE);
		if (optlen > 3) {
			if (fqdn_flags & F_FQDN_E) {
				get_dns_name(tvb, optoff+3, optlen-3, optoff+3, &dns_name);
				proto_tree_add_string(v_tree, hf_bootp_fqdn_name, 
				    tvb, optoff+3, optlen-3, dns_name);
			} else {
				proto_tree_add_item(v_tree, hf_bootp_fqdn_asciiname,
				    tvb, optoff+3, optlen-3, FALSE);
			}
		}
		break;

	case 82:        /
		optend = optoff + optlen;
		while (optoff < optend)
			optoff = bootp_dhcp_decode_agent_info(v_tree, tvb, optoff, optend);
		break;

	case 85:        /
		/
		/
		if (novell_string) {
			proto_item_append_text(vti, " = \"%s\"",
			    tvb_format_stringzpad(tvb, optoff, optlen));
		} else {
			if (optlen == 4) {
				/
				proto_item_append_text(vti, " = %s",
					ip_to_str(tvb_get_ptr(tvb, optoff, 4)));
			} else {
				/
				for (i = optoff, optleft = optlen; optleft > 0;
				    i += 4, optleft -= 4) {
					if (optleft < 4) {
						proto_tree_add_text(v_tree, tvb, i, optleft,
						    "Option length isn't a multiple of 4");
						break;
					}
					proto_tree_add_text(v_tree, tvb, i, 4, "IP Address: %s",
						ip_to_str(tvb_get_ptr(tvb, i, 4)));
				}
			}
        	}
	        break;

	case 94: {	/
		guint8 id_type;

		id_type = tvb_get_guint8(tvb, optoff);

		if (id_type == 0x01) {
			proto_tree_add_item(v_tree, hf_bootp_client_network_id_major_ver,
					    tvb, optoff + 1, 1, TRUE);
			proto_tree_add_item(v_tree, hf_bootp_client_network_id_minor_ver,
					    tvb, optoff + 2, 1, TRUE);
		}

		break;
	}

	case 90:	/
	case 210:	/
		if (optlen < 11) {
			proto_item_append_text(vti, " length isn't >= 11");
			break;
		}
		optleft = optlen;
		protocol = tvb_get_guint8(tvb, optoff);
		proto_tree_add_text(v_tree, tvb, optoff, 1, "Protocol: %s (%u)",
				    val_to_str(protocol, authen_protocol_vals, "Unknown"),
				    protocol);
		optoff++;
		optleft--;

		algorithm = tvb_get_guint8(tvb, optoff);
		switch (protocol) {

		case AUTHEN_PROTO_DELAYED_AUTHEN:
			proto_tree_add_text(v_tree, tvb, optoff, 1,
				    "Algorithm: %s (%u)",
				    val_to_str(algorithm, authen_da_algo_vals, "Unknown"),
				    algorithm);
			break;

		default:
			proto_tree_add_text(v_tree, tvb, optoff, 1,
				    "Algorithm: %u", algorithm);
			break;
		}
		optoff++;
		optleft--;

		rdm = tvb_get_guint8(tvb, optoff);
		proto_tree_add_text(v_tree, tvb, optoff, 1,
				    "Replay Detection Method: %s (%u)",
				    val_to_str(rdm, authen_rdm_vals, "Unknown"),
				    rdm);
		optoff++;
		optleft--;

		switch (rdm) {

		case AUTHEN_RDM_MONOTONIC_COUNTER:
			proto_tree_add_text(v_tree, tvb, optoff, 8,
				    "RDM Replay Detection Value: %" G_GINT64_MODIFIER "x",
				    tvb_get_ntoh64(tvb, optoff));
			break;

		default:
			proto_tree_add_text(v_tree, tvb, optoff, 8,
				    "Replay Detection Value: %s",
				    tvb_bytes_to_str(tvb, optoff, 8));
			break;
		}
		optoff += 8;
		optleft -= 8;

		switch (protocol) {

		case AUTHEN_PROTO_DELAYED_AUTHEN:
			switch (algorithm) {

			case AUTHEN_DELAYED_ALGO_HMAC_MD5:
				if (*dhcp_type_p && !strcmp(*dhcp_type_p, OPT53_DISCOVER)) {
					/
					break;
				} else {
					if (optlen < 31) {
						proto_item_append_text(vti,
							" length isn't >= 31");
						break;
					}
					proto_tree_add_text(v_tree, tvb, optoff, 4,
						"Secret ID: 0x%08x",
						tvb_get_ntohl(tvb, optoff));
					optoff += 4;
					optleft -= 4;
					proto_tree_add_text(v_tree, tvb, optoff, 16,
						"HMAC MD5 Hash: %s",
						tvb_bytes_to_str(tvb, optoff, 16));
					break;
				}

			default:
				if (optleft == 0)
					break;
				proto_tree_add_text(v_tree, tvb, optoff, optleft,
					"Authentication Information: %s",
					tvb_bytes_to_str(tvb, optoff, optleft));
				break;
			}
			break;

		default:
			if (optleft == 0)
				break;
			proto_tree_add_text(v_tree, tvb, optoff, optleft,
				"Authentication Information: %s",
				tvb_bytes_to_str(tvb, optoff, optleft));
			break;
		}
		break;

	case 99: /

		optleft = optlen;
		if (optleft >= 3)
		{
			proto_tree_add_text(v_tree, tvb, optoff, 1, "What: %d (%s)",
				tvb_get_guint8(tvb, optoff), val_to_str(tvb_get_guint8(tvb, optoff),
				civic_address_what_values, "Unknown") );
			proto_tree_add_text(v_tree, tvb, optoff + 1, 2, "Country: \"%s\"",
				tvb_format_text(tvb, optoff + 1, 2) );
			optleft = optleft - 3;
			optoff = optoff + 3;

			while (optleft >= 2)
			{
				int catype = tvb_get_guint8(tvb, optoff);
				optoff++;
				optleft--;
				s_option = tvb_get_guint8(tvb, optoff);
				optoff++;
				optleft--;

				if (s_option == 0)
				{
					proto_tree_add_text(v_tree, tvb, optoff, s_option,
						"CAType %d [%s] (l=%d): EMTPY", catype,
						val_to_str(catype, civic_address_type_values,
						"Unknown"), s_option);
					continue;
				}

				if (optleft >= s_option)
				{
					proto_tree_add_text(v_tree, tvb, optoff, s_option,
						"CAType %d [%s] (l=%d): \"%s\"", catype,
						val_to_str(catype, civic_address_type_values,
						"Unknown"), s_option,
						tvb_format_text(tvb, optoff, s_option));
					optoff = optoff + s_option;
					optleft = optleft - s_option;
				}
				else
				{
					optleft = 0;
					proto_tree_add_text(v_tree, tvb, optoff, s_option,
						"Error with CAType");
				}
			}
		}

		break;

	case 121:	/
	case 249: {	/
		int mask_width, significant_octets;
		optend = optoff + optlen;
		/
		if (optlen < 5) {
			proto_item_append_text(vti, " [ERROR: Option length < 5 bytes]");
			break;
		}
		while (optoff < optend) {
			mask_width = tvb_get_guint8(tvb, optoff);
			/
			if (mask_width > 32) {
				proto_tree_add_text(v_tree, tvb, optoff,
					optend - optoff,
					"Subnet/MaskWidth-Router: [ERROR: Mask width (%d) > 32]",
					mask_width);
				break;
			}
			significant_octets = (mask_width + 7) / 8;
			vti = proto_tree_add_text(v_tree, tvb, optoff,
				1 + significant_octets + 4,
				"Subnet/MaskWidth-Router: ");
			optoff++;
			/
			if (optend < optoff + significant_octets + 4) {
				proto_item_append_text(vti, "[ERROR: Remaining length (%d) < %d bytes]",
					optend - optoff, significant_octets + 4);
				break;
			}
			if(mask_width == 0)
				proto_item_append_text(vti, "default");
			else {
				for(i = 0 ; i < significant_octets ; i++) {
					if (i > 0)
						proto_item_append_text(vti, ".");
					byte = tvb_get_guint8(tvb, optoff++);
					proto_item_append_text(vti, "%d", byte);
				}
				for(i = significant_octets ; i < 4 ; i++)
					proto_item_append_text(vti, ".0");
				proto_item_append_text(vti, "/%d", mask_width);
			}
			proto_item_append_text(vti, "-%s",
			    ip_to_str(tvb_get_ptr(tvb, optoff, 4)));
			optoff += 4;
		}
		break;
	}

	case 123: /
		if (optlen == 16) {
			int c;
			unsigned char lci[16];
			struct rfc3825_location_fixpoint_t location_fp;
			struct rfc3825_location_decimal_t location;

			for (c=0; c < 16;c++)
				lci[c] = (unsigned char) tvb_get_guint8(tvb, optoff + c);

			/
			rfc3825_lci_to_fixpoint(lci, &location_fp);

			/
			i = rfc3825_fixpoint_to_decimal(&location_fp, &location);

			if (i != RFC3825_NOERROR) {
				proto_tree_add_text(v_tree, tvb, optoff, optlen, "Error: %s", val_to_str(i, rfc3825_error_types, "Unknown"));
			} else {
				proto_tree_add_text(v_tree, tvb, optoff, 5, "Latitude: %15.10f", location.latitude);
				proto_tree_add_text(v_tree, tvb, optoff+5, 5, "Longitude: %15.10f", location.longitude);
				proto_tree_add_text(v_tree, tvb, optoff, 1, "Latitude resolution: %15.10f", location.latitude_res);
				proto_tree_add_text(v_tree, tvb, optoff+5, 1, "Longitude resolution: %15.10f", location.longitude_res);
				proto_tree_add_text(v_tree, tvb, optoff+12, 4, "Altitude: %15.10f", location.altitude);
				proto_tree_add_text(v_tree, tvb, optoff+10, 2, "Altitude resolution: %15.10f", location.altitude_res);
				proto_tree_add_text(v_tree, tvb, optoff+10, 1, "Altitude type: %s (%d)", val_to_str(location.altitude_type, altitude_type_values, "Unknown"), location.altitude_type);
				proto_tree_add_text(v_tree, tvb, optoff+15, 1, "Map Datum: %s (%d)", val_to_str(location.datum_type, map_datum_type_values, "Unknown"), location.datum_type);
			}
		} else if (optlen == 34) {
			s_option = tvb_get_guint8(tvb, optoff);
			s_len = tvb_get_guint8(tvb, optoff+1);
			if (s_option == 1) {
				proto_tree_add_text(v_tree, tvb, optoff, optlen, "Suboption 1: Primary DSS_ID = \"%s\"",
					tvb_format_stringzpad(tvb, optoff+2, s_len));
			} else if (s_option == 2) {
				proto_tree_add_text(v_tree, tvb, optoff, optlen, "Suboption 2: Secondary DSS_ID = \"%s\"", 
					tvb_format_stringzpad(tvb, optoff+2, s_len));
			} else {
				proto_tree_add_text(v_tree, tvb, optoff, optlen, "Unknown");
			}
		} else {
			proto_tree_add_text(v_tree, tvb, optoff, optlen, "Error: Invalid length of DHCP option!");
		}
		break;

	case 124: { 	/
	        int enterprise = 0;
		int data_len;

		if (optlen == 1) {
			/
			s_option = tvb_get_guint8(tvb, optoff);
			proto_tree_add_text(v_tree, tvb, optoff, optlen, 
					    "CableLabs IP addressing mode preference: %s",
					    val_to_str (s_option, cablelab_ipaddr_mode_vals, "Unknown"));
			break;
		}

		optend = optoff + optlen;
	        optleft = optlen;

		while (optleft > 0) {

		  if (optleft < 5) {
		    proto_tree_add_text(v_tree, tvb, optoff,
					optleft, "Vendor Class: malformed option");
		    break;
		  }

		  enterprise = tvb_get_ntohl(tvb, optoff);

		  vti = proto_tree_add_text(v_tree, tvb, optoff, 4,
					    "Enterprise-number: %s (%u)",
					    val_to_str(enterprise, sminmpec_values, "Unknown"),
					    enterprise);

		  data_len = tvb_get_guint8(tvb, optoff + 4);

		  proto_tree_add_text(v_tree, tvb, optoff + 4, 1,
				      "Data len: %d", data_len);
		  optoff += 5;
		  optleft -= 5;

		  proto_tree_add_text(v_tree, tvb, optoff, data_len,
				      "Vendor Class data: %s",
				      tvb_bytes_to_str(tvb, optoff, data_len));

		  /
		  optoff += data_len;
		  optleft -= data_len;
		}
		break;
	}

	case 125: { 	/
	        int enterprise = 0;
		int s_end = 0;
		int s_option_len = 0;
		proto_tree *e_tree = 0;

		optend = optoff + optlen;

	        optleft = optlen;

		while (optleft > 0) {

		  if (optleft < 5) {
		    proto_tree_add_text(v_tree, tvb, optoff,
					optleft, "Vendor-specific Information: malformed option");
		    break;
		  }

		  enterprise = tvb_get_ntohl(tvb, optoff);

		  vti = proto_tree_add_text(v_tree, tvb, optoff, 4,
					    "Enterprise-number: %s (%u)",
					    val_to_str( enterprise, sminmpec_values, "Unknown"),
					    enterprise);

		  s_option_len = tvb_get_guint8(tvb, optoff + 4);

		  optoff += 5;
		  optleft -= 5;

		  /
		  switch (enterprise) {

		  case 3561: /
		    s_end = optoff + s_option_len;
		    if ( s_end > optend ) {
		      proto_tree_add_text(v_tree, tvb, optoff, 1,
					  "no room left in option for enterprise %u data", enterprise);
		      break;
		    }


		    e_tree = proto_item_add_subtree(vti, ett_bootp_option);
		    while (optoff < s_end) {

		      optoff = dissect_vendor_tr111_suboption(e_tree,
								 tvb, optoff, s_end);
		    }

		  case 4491: /
		    s_end = optoff + s_option_len;
		    if ( s_end > optend ) {
		      proto_tree_add_text(v_tree, tvb, optoff, 1,
					  "no room left in option for enterprise %u data", enterprise);
		      break;
		    }

		    e_tree = proto_item_add_subtree(vti, ett_bootp_option);
		    while (optoff < s_end) {
		      optoff = dissect_vendor_cl_suboption(e_tree,
								 tvb, optoff, s_end);
		    }

		  default:
		    /
		    optoff += s_option_len;
		  }

		  optleft -= s_option_len;

		}
		break;
	}

	default:	/
		/
		if (code == pkt_ccc_option) {
			ftype = special;
			proto_item_append_text(vti,
				"CableLabs Client Configuration (%d bytes)",
				optlen);
			optend = optoff + optlen;
			while (optoff < optend) {
				switch (pkt_ccc_protocol_version) {

				case PACKETCABLE_CCC_I05:
					optoff = dissect_packetcable_i05_ccc(v_tree, tvb, optoff, optend);
					break;
				case PACKETCABLE_CCC_DRAFT5:
				case PACKETCABLE_CCC_RFC_3495:
					optoff = dissect_packetcable_ietf_ccc(v_tree, tvb, optoff, optend, pkt_ccc_protocol_version);
					break;
				default: /
					break;
				}
			}
		}

		break;
	}

	switch (ftype) {

	case ipv4:
		if (optlen != 4) {
			proto_item_append_text(vti,
			    " - length isn't 4");
			break;
		}
		proto_item_append_text(vti, " = %s",
			ip_to_str(tvb_get_ptr(tvb, optoff, 4)));
		break;

	case ipv4_list:
		if (optlen == 4) {
			/
			proto_item_append_text(vti, " = %s",
				ip_to_str(tvb_get_ptr(tvb, optoff, 4)));
		} else {
			/
			for (i = optoff, optleft = optlen; optleft > 0;
			    i += 4, optleft -= 4) {
				if (optleft < 4) {
					proto_tree_add_text(v_tree, tvb, i, voff + consumed - i,
					    "Option length isn't a multiple of 4");
					break;
				}
				proto_tree_add_text(v_tree, tvb, i, 4, "IP Address: %s",
					ip_to_str(tvb_get_ptr(tvb, i, 4)));
			}
		}
		break;

	case string:
		/
		proto_item_append_text(vti, " = \"%s\"",
				tvb_format_stringzpad(tvb, optoff, consumed-2));
		break;

	case val_boolean:
		if (optlen != 1) {
			proto_item_append_text(vti,
			    " - length isn't 1");
			break;
		}
		tfs = (const struct true_false_string *) bootp_get_opt_data(code);
		if(tfs){
			i = tvb_get_guint8(tvb, optoff);
			if (i != 0 && i != 1) {
				proto_item_append_text(vti,
				    " = Invalid Value %d", i);
			} else {
				proto_item_append_text(vti, " = %s",
			    		i == 0 ? tfs->false_string : tfs->true_string);
			}
		}
		break;

	case val_u_byte:
		if (optlen != 1) {
			proto_item_append_text(vti,
			    " - length isn't 1");
			break;
		}
		vs = (const value_string *) bootp_get_opt_data(code);
		byte = tvb_get_guint8(tvb, optoff);
		if (vs != NULL) {
			proto_item_append_text(vti, " = %s",
			    val_to_str(byte, vs, "Unknown (%u)"));
		} else
			proto_item_append_text(vti, " = %u", byte);
		break;

	case val_u_short: {
		gushort vd;

		if (optlen != 2) {
			proto_item_append_text(vti,
			    " - length isn't 2");
			break;
		}

		vs = (const value_string *) bootp_get_opt_data(code);
		vd = tvb_get_ntohs(tvb, optoff);

		if (vs != NULL) {
			proto_item_append_text(vti, " = %s",
                            val_to_str(vd, vs, "Unknown (%u)"));
		} else
			proto_item_append_text(vti, " = %u", vd);

		break;
	}

	case val_u_short_list:
		if (optlen == 2) {
			/
			proto_item_append_text(vti, " = %u",
			    tvb_get_ntohs(tvb, optoff));
		} else {
			/
			for (i = optoff, optleft = optlen; optleft > 0;
			    i += 2, optleft -= 2) {
				if (optleft < 2) {
					proto_tree_add_text(v_tree, tvb, i, voff + consumed - i,
					    "Option length isn't a multiple of 2");
					break;
				}
				proto_tree_add_text(v_tree, tvb, i, 4, "Value: %u",
					tvb_get_ntohs(tvb, i));
			}
		}
		break;

	case val_u_long:
		if (optlen != 4) {
			proto_item_append_text(vti,
			    " - length isn't 4");
			break;
		}
		proto_item_append_text(vti, " = %u",
		    tvb_get_ntohl(tvb, optoff));
		break;

	case time_in_s_secs:
		if (optlen != 4) {
			proto_item_append_text(vti,
			    " - length isn't 4");
			break;
		}
		time_s_secs = (gint32) tvb_get_ntohl(tvb, optoff);
		proto_item_append_text(vti, " = %s",
		      time_secs_to_str(time_s_secs));
		break;

	case time_in_u_secs:
		if (optlen != 4) {
			proto_item_append_text(vti,
			    " - length isn't 4");
			break;
		}
		time_u_secs = tvb_get_ntohl(tvb, optoff);
		proto_item_append_text(vti, " = %s",
		    ((time_u_secs == 0xffffffff) ?
		      "infinity" :
		      time_secs_to_str_unsigned(time_u_secs)));
		break;

	default:
		break;
	}

	return consumed;
}
