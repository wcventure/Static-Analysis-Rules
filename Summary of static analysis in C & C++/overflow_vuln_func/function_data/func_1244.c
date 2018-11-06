guint
dissect_snmp_pdu(tvbuff_t *tvb, int offset, packet_info *pinfo,
    proto_tree *tree, int proto, gint ett, gboolean is_tcp)
{
	guint length_remaining;
	ASN1_SCK asn1;
	int start;
	gboolean def;
	gboolean encrypted;
	guint length;
	guint message_length;
	guint global_length;

	guint32 version;
	guint32 msgid;
	guint32 msgmax;
	guint32 msgsec;
	guint32 engineboots;
	guint32 enginetime;

	guchar *msgflags;
	gchar *commustr;
	guchar *community;
	guchar *secparm;
	guchar *cengineid;
	guchar *cname;
	guchar *cryptpdu;
	guchar *aengineid;
	guchar *username;
	guchar *authpar;
	guchar *privpar;
	guint msgflags_length;
	guint community_length;
	guint secparm_length;
	guint cengineid_length;
	guint cname_length;
	guint cryptpdu_length;
	guint aengineid_length;
	guint username_length;
	guint authpar_length;
	guint privpar_length;

	guint pdu_type;
	guint pdu_length;

	proto_tree *snmp_tree = NULL;
	proto_tree *global_tree = NULL;
	proto_tree *flags_tree = NULL;
	proto_tree *secur_tree = NULL;
	proto_item *item = NULL;
	int ret;
	guint cls, con, tag;

	/
	length_remaining = tvb_ensure_length_remaining(tvb, offset);

	/

	/
	if (is_tcp && snmp_desegment && pinfo->can_desegment) {
		/
		if (length_remaining < 6) {
			pinfo->desegment_offset = offset;
			pinfo->desegment_len = 6 - length_remaining;

			/
			return 0;
		}
	}

	/
	asn1_open(&asn1, tvb, offset);
	ret = asn1_sequence_decode(&asn1, &message_length, &length);
	if (ret != ASN1_ERR_NOERROR) {
		if (tree) {
			item = proto_tree_add_item(tree, proto, tvb, offset,
			    -1, FALSE);
			snmp_tree = proto_item_add_subtree(item, ett);
		}
		dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			"message header", ret);

		/
		return length_remaining;
	}

	/
	message_length += length;
	if (message_length < length) {
		/
		show_reported_bounds_error(tvb, pinfo, tree);

		/
		return length_remaining;
	}

	/
	if (is_tcp && snmp_desegment && pinfo->can_desegment) {
		/
		if (length_remaining < message_length) {
			/
			pinfo->desegment_offset = offset;
			pinfo->desegment_len =
			    message_length - length_remaining;

			/
			return 0;
		}
	}

	if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
		col_set_str(pinfo->cinfo, COL_PROTOCOL,
		    proto_get_protocol_short_name(find_protocol_by_id(proto)));
	}

	if (tree) {
		item = proto_tree_add_item(tree, proto, tvb, offset,
		    message_length, FALSE);
		snmp_tree = proto_item_add_subtree(item, ett);
	}
	offset += length;

	ret = asn1_uint32_decode (&asn1, &version, &length);
	if (ret != ASN1_ERR_NOERROR) {
		dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
		    "version number", ret);
		return message_length;
	}
	if (snmp_tree) {
		proto_tree_add_uint(snmp_tree, hf_snmp_version, tvb, offset,
		    length, version);
	}
	offset += length;


	switch (version) {
	case SNMP_VERSION_1:
	case SNMP_VERSION_2c:
		ret = asn1_octet_string_decode (&asn1, &community,
		    &community_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "community", ret);
			return message_length;
		}
		if (tree) {
			commustr = g_malloc(community_length+1);
			memcpy(commustr, community, community_length);
			commustr[community_length] = '\0';

			proto_tree_add_string(snmp_tree, hf_snmp_community,
			    tvb, offset, length, commustr);
			g_free(commustr);
		}
		g_free(community);
		offset += length;
		break;
	case SNMP_VERSION_2u:
		ret = asn1_octet_string_decode (&asn1, &community,
		    &community_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
				"community (2u)", ret);
			return message_length;
		}
		if (tree) {
			dissect_snmp2u_parameters(snmp_tree, tvb, offset, length,
			    community, community_length);
		}
		g_free(community);
		offset += length;
		break;
	case SNMP_VERSION_3:
		ret = asn1_sequence_decode(&asn1, &global_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
				"message global header", ret);
			return message_length;
		}
		if (snmp_tree) {
			item = proto_tree_add_text(snmp_tree, tvb, offset,
			    global_length + length, "Message Global Header");
			global_tree = proto_item_add_subtree(item, ett_global);
			proto_tree_add_text(global_tree, tvb, offset,
		 	    length,
			    "Message Global Header Length: %d", global_length);
		}
		offset += length;
		ret = asn1_uint32_decode (&asn1, &msgid, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "message id", ret);
			return message_length;
		}
		if (global_tree) {
			proto_tree_add_text(global_tree, tvb, offset,
			    length, "Message ID: %d", msgid);
		}
		offset += length;
		ret = asn1_uint32_decode (&asn1, &msgmax, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "message max size", ret);
			return message_length;
		}
		if (global_tree) {
			proto_tree_add_text(global_tree, tvb, offset,
			    length, "Message Max Size: %d", msgmax);
		}
		offset += length;
		ret = asn1_octet_string_decode (&asn1, &msgflags,
		    &msgflags_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "message flags", ret);
			return message_length;
		}
		if (msgflags_length != 1) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "message flags wrong length", ret);
			g_free(msgflags);
			return message_length;
		}
		if (global_tree) {
			item = proto_tree_add_uint_format(global_tree,
			    hf_snmpv3_flags, tvb, offset, length,
			    msgflags[0], "Flags: 0x%02x", msgflags[0]);
			flags_tree = proto_item_add_subtree(item, ett_flags);
			proto_tree_add_boolean(flags_tree, hf_snmpv3_flags_report,
			    tvb, offset, length, msgflags[0]);
			proto_tree_add_boolean(flags_tree, hf_snmpv3_flags_crypt,
			    tvb, offset, length, msgflags[0]);
			proto_tree_add_boolean(flags_tree, hf_snmpv3_flags_auth,
			    tvb, offset, length, msgflags[0]);
		}
		encrypted = msgflags[0] & TH_CRYPT;
		g_free(msgflags);
		offset += length;
		ret = asn1_uint32_decode (&asn1, &msgsec, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "message security model", ret);
			return message_length;
		}
		if (global_tree) {
			proto_tree_add_text(global_tree, tvb, offset,
			    length, "Message Security Model: %s",
			    val_to_str(msgsec, sec_models,
			    "Unknown model %#x"));
		}
		offset += length;
		switch(msgsec) {
		case SNMP_SEC_USM:
			start = asn1.offset;
			ret = asn1_header_decode (&asn1, &cls, &con, &tag,
			    &def, &secparm_length);
			length = asn1.offset - start;
			if (cls != ASN1_UNI && con != ASN1_PRI &&
			    tag != ASN1_OTS) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "Message Security Parameters",
				    ASN1_ERR_WRONG_TYPE);
				return message_length;
			}
			if (snmp_tree) {
				item = proto_tree_add_text(snmp_tree, tvb,
				    offset, secparm_length + length,
				    "Message Security Parameters");
				secur_tree = proto_item_add_subtree(item,
				    ett_secur);
				proto_tree_add_text(secur_tree, tvb, offset,
			 	    length,
				    "Message Security Parameters Length: %d",
				    secparm_length);
			}
			offset += length;
			ret = asn1_sequence_decode(&asn1, &secparm_length,
			    &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "USM sequence header", ret);
				return message_length;
			}
			offset += length;
			ret = asn1_octet_string_decode (&asn1, &aengineid,
			    &aengineid_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "authoritative engine id", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb, offset,
				    length, "Authoritative Engine ID: %s",
				    bytes_to_str(aengineid, aengineid_length));
			}
			g_free(aengineid);
			offset += length;
			ret = asn1_uint32_decode (&asn1, &engineboots, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "engine boots", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb,
				    offset, length, "Engine Boots: %d",
				    engineboots);
			}
			offset += length;
			ret = asn1_uint32_decode (&asn1, &enginetime, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree,  "engine time", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb,
				    offset, length, "Engine Time: %d",
				    enginetime);
			}
			offset += length;
			ret = asn1_octet_string_decode (&asn1, &username,
			    &username_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "user name", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb, offset,
				    length, "User Name: %s",
				    SAFE_STRING(username, username_length));
			}
			g_free(username);
			offset += length;
			ret = asn1_octet_string_decode (&asn1, &authpar,
			    &authpar_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "authentication parameter", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb, offset,
				    length, "Authentication Parameter: %s",
				    bytes_to_str(authpar, authpar_length));
			}
			g_free(authpar);
			offset += length;
			ret = asn1_octet_string_decode (&asn1, &privpar,
			    &privpar_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "privacy parameter", ret);
				return message_length;
			}
			if (secur_tree) {
				proto_tree_add_text(secur_tree, tvb, offset,
				    length, "Privacy Parameter: %s",
				    bytes_to_str(privpar, privpar_length));
			}
			g_free(privpar);
			offset += length;
			break;
		default:
			ret = asn1_octet_string_decode (&asn1,
			    &secparm, &secparm_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "Message Security Parameters",
				    ret);
				return message_length;
			}
			if (snmp_tree) {
				proto_tree_add_text(snmp_tree, tvb, offset,
				    length,
				    "Message Security Parameters Data"
				    " (%d bytes)", secparm_length);
			}
			g_free(secparm);
			offset += length;
			break;
		}
		/
		if (encrypted) {
			ret = asn1_octet_string_decode (&asn1, &cryptpdu,
			    &cryptpdu_length, &length);
			if (ret != ASN1_ERR_NOERROR) {
				dissect_snmp_parse_error(tvb, offset, pinfo,
				    snmp_tree, "encrypted PDU header", ret);
				return message_length;
			}
			proto_tree_add_text(snmp_tree, tvb, offset, length,
			    "Encrypted PDU (%d bytes)", length);
			g_free(cryptpdu);
			if (check_col(pinfo->cinfo, COL_INFO))
				col_set_str(pinfo->cinfo, COL_INFO, "Encrypted PDU");
			return message_length;
		}
		ret = asn1_sequence_decode(&asn1, &global_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
				"PDU header", ret);
			return message_length;
		}
		offset += length;
		ret = asn1_octet_string_decode (&asn1, &cengineid,
		    &cengineid_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "context engine id", ret);
			return message_length;
		}
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, tvb, offset, length,
			    "Context Engine ID: %s",
			    bytes_to_str(cengineid, cengineid_length));
		}
		g_free(cengineid);
		offset += length;
		ret = asn1_octet_string_decode (&asn1, &cname,
		    &cname_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
			    "context name", ret);
			return message_length;
		}
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, tvb, offset, length,
			    "Context Name: %s",
			    SAFE_STRING(cname, cname_length));
		}
		g_free(cname);
		offset += length;
		break;
	default:
		dissect_snmp_error(tvb, offset, pinfo, snmp_tree,
		    "PDU for unknown version of SNMP");
		return message_length;
	}

	start = asn1.offset;
	ret = asn1_header_decode (&asn1, &cls, &con, &pdu_type, &def,
	    &pdu_length);
	if (ret != ASN1_ERR_NOERROR) {
		dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
		    "PDU type", ret);
		return message_length;
	}
	if (cls != ASN1_CTX || con != ASN1_CON) {
		dissect_snmp_parse_error(tvb, offset, pinfo, snmp_tree,
		    "PDU type", ASN1_ERR_WRONG_TYPE);
		return message_length;
	}
	dissect_common_pdu(tvb, offset, pinfo, snmp_tree, tree, asn1, pdu_type, start);
	return message_length;
}
