static void
dissect_smux_pdu(tvbuff_t *tvb, int offset, packet_info *pinfo,
    proto_tree *tree, int proto, gint ett)
{
	ASN1_SCK asn1;
	int start;
	gboolean def;
	guint length;

	guint pdu_type;
	char *pdu_type_string;
	guint pdu_length;

	guint32 version;
	guint32 cause;
	guint32 priority;
	guint32 operation;
	guint32 commit;

	guchar *password;
	guint password_length;

	guchar *application;
	guint application_length;

	subid_t *regid;
	guint regid_length;

	gchar *oid_string;

	proto_tree *smux_tree = NULL;
	proto_item *item = NULL;
	int ret;
	guint cls, con;

	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "SMUX");

	if (tree) {
		item = proto_tree_add_item(tree, proto, tvb, offset, -1, FALSE);
		smux_tree = proto_item_add_subtree(item, ett);
	}

	/
	/
	asn1_open(&asn1, tvb, offset);
	start = asn1.offset;
	ret = asn1_header_decode (&asn1, &cls, &con, &pdu_type, &def,
	    &pdu_length);
	if (ret != ASN1_ERR_NOERROR) {
		dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
		    "PDU type", ret);
		return;
	}

	/
	if (cls == ASN1_APL && con == ASN1_CON && pdu_type == SMUX_MSG_OPEN) {
		pdu_type_string = val_to_str(pdu_type, smux_types,
		    "Unknown PDU type %#x");
		if (check_col(pinfo->cinfo, COL_INFO))
			col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_pdutype, tvb,
			    offset, length, pdu_type);
		}
		offset += length;
		ret = asn1_uint32_decode (&asn1, &version, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "version", ret);
			return;
		}
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_version, tvb,
			    offset, length, version);
		}
		offset += length;

		ret = asn1_oid_decode (&asn1, &regid, &regid_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "registration OID", ret);
			return;
		}
		if (tree) {
			oid_string = format_oid(regid, regid_length);
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Registration: %s", oid_string);
			g_free(oid_string);
		}
		g_free(regid);
		offset += length;

		ret = asn1_octet_string_decode (&asn1, &application,
		    &application_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "application", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Application: %s",
			     SAFE_STRING(application, application_length));
		}
		g_free(application);
		offset += length;

		ret = asn1_octet_string_decode (&asn1, &password,
		    &password_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "password", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Password: %s",
			    SAFE_STRING(password, password_length));
		}
		g_free(password);
		offset += length;
		return;
	}
	if (cls == ASN1_APL && con == ASN1_PRI && pdu_type == SMUX_MSG_CLOSE) {
		pdu_type_string = val_to_str(pdu_type, smux_types,
		    "Unknown PDU type %#x");
		if (check_col(pinfo->cinfo, COL_INFO))
			col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_pdutype, tvb,
			    offset, length, pdu_type);
		}
		offset += length;
		ret = asn1_uint32_value_decode (&asn1, pdu_length, &cause);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "cause", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset,
			    pdu_length, "Cause: %s",
			    val_to_str(cause, smux_close,
				"Unknown cause %#x"));
		}
		offset += pdu_length;
		return;
	}
	if (cls == ASN1_APL && con == ASN1_CON && pdu_type == SMUX_MSG_RREQ) {
		pdu_type_string = val_to_str(pdu_type, smux_types,
		    "Unknown PDU type %#x");
		if (check_col(pinfo->cinfo, COL_INFO))
			col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_pdutype, tvb,
			    offset, length, pdu_type);
		}
		offset += length;
		ret = asn1_oid_decode (&asn1, &regid, &regid_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "registration subtree", ret);
			return;
		}
		if (tree) {
			oid_string = format_oid(regid, regid_length);
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Registration: %s", oid_string);
			g_free(oid_string);
		}
		g_free(regid);
		offset += length;

		ret = asn1_uint32_decode (&asn1, &priority, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "priority", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Priority: %d", priority);
		}
		offset += length;

		ret = asn1_uint32_decode (&asn1, &operation, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "operation", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset, length,
			    "Operation: %s",
			    val_to_str(operation, smux_rreq,
				"Unknown operation %#x"));
		}
		offset += length;
		return;
	}
	if (cls == ASN1_APL && con == ASN1_PRI && pdu_type == SMUX_MSG_RRSP) {
		pdu_type_string = val_to_str(pdu_type, smux_types,
		    "Unknown PDU type %#x");
		if (check_col(pinfo->cinfo, COL_INFO))
			col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_pdutype, tvb,
			    offset, length, pdu_type);
		}
		offset += length;
		ret = asn1_uint32_value_decode (&asn1, pdu_length, &priority);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "priority", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset,
			    pdu_length, "%s",
			    val_to_str(priority, smux_prio,
				"Priority: %#x"));
		}
		offset += pdu_length;
		return;
	}
	if (cls == ASN1_APL && con == ASN1_PRI && pdu_type == SMUX_MSG_SOUT) {
		pdu_type_string = val_to_str(pdu_type, smux_types,
		    "Unknown PDU type %#x");
		if (check_col(pinfo->cinfo, COL_INFO))
			col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_uint(smux_tree, hf_smux_pdutype, tvb,
			    offset, length, pdu_type);
		}
		offset += length;
		ret = asn1_uint32_value_decode (&asn1, pdu_length, &commit);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
			    "commit", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(smux_tree, tvb, offset,
			    pdu_length, "%s",
			    val_to_str(commit, smux_sout,
				"Unknown SOUT Value: %#x"));
		}
		offset += pdu_length;
		return;
	}
	if (cls != ASN1_CTX || con != ASN1_CON) {
		dissect_snmp_parse_error(tvb, offset, pinfo, smux_tree,
		    "PDU type", ASN1_ERR_WRONG_TYPE);
		return;
	}
	dissect_common_pdu(tvb, offset, pinfo, smux_tree, tree, asn1, pdu_type, start);
}
