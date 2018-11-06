static void
dissect_common_pdu(tvbuff_t *tvb, int offset, packet_info *pinfo,
    proto_tree *tree, ASN1_SCK asn1, guint pdu_type, int start)
{
	gboolean def;
	guint length;
	guint sequence_length;

	guint32 request_id;

	guint32 error_status;

	guint32 error_index;

	char *pdu_type_string;

	subid_t *enterprise;
	guint enterprise_length;

	guint8 *agent_address;
	guint agent_address_length;

	guint32 trap_type;

	guint32 specific_type;

	guint timestamp;
	guint timestamp_length;

	gchar *oid_string;

	guint variable_bindings_length;

	int vb_index;
	guint variable_length;
	subid_t *variable_oid;
	guint variable_oid_length;
#if defined(HAVE_UCD_SNMP_SNMP_H) || defined(HAVE_SNMP_SNMP_H)
	gchar vb_oid_string[MAX_STRING_LEN]; /
#endif
	gboolean unsafe;

	int ret;
	guint cls, con, tag;

	pdu_type_string = val_to_str(pdu_type, pdu_types,
	    "Unknown PDU type %#x");
	if (check_col(pinfo->cinfo, COL_INFO))
		col_add_str(pinfo->cinfo, COL_INFO, pdu_type_string);
	length = asn1.offset - start;
	if (tree) {
		proto_tree_add_text(tree, tvb, offset, length,
		    "PDU type: %s", pdu_type_string);
	}
	offset += length;

	/
	switch (pdu_type) {

	case SNMP_MSG_GET:
	case SNMP_MSG_GETNEXT:
	case SNMP_MSG_RESPONSE:
	case SNMP_MSG_SET:
	case SNMP_MSG_GETBULK:
	case SNMP_MSG_INFORM:
	case SNMP_MSG_TRAP2:
	case SNMP_MSG_REPORT:
		/
		ret = asn1_uint32_decode (&asn1, &request_id, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "request ID", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(tree, tvb, offset, length,
			    "Request Id: %#x", request_id);
		}
		offset += length;
		
		/
		ret = asn1_uint32_decode (&asn1, &error_status, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    (pdu_type == SNMP_MSG_GETBULK) ? "non-repeaters"
			    				   : "error status",
			    ret);
			return;
		}
		if (tree) {
			if (pdu_type == SNMP_MSG_GETBULK) {
				proto_tree_add_text(tree, tvb, offset,
				    length, "Non-repeaters: %u", error_status);
			} else {
				proto_tree_add_text(tree, tvb, offset,
				    length, "Error Status: %s",
				    val_to_str(error_status, error_statuses,
				      "Unknown (%d)"));
			}
		}
		offset += length;

		/
		ret = asn1_uint32_decode (&asn1, &error_index, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    (pdu_type == SNMP_MSG_GETBULK) ? "max repetitions"
			    				   : "error index",
			    ret);
			return;
		}
		if (tree) {
			if (pdu_type == SNMP_MSG_GETBULK) {
				proto_tree_add_text(tree, tvb, offset,
				    length, "Max repetitions: %u", error_index);
			} else {
				proto_tree_add_text(tree, tvb, offset,
				    length, "Error Index: %u", error_index);
			}
		}
		offset += length;
		break;

	case SNMP_MSG_TRAP:
		/
		ret = asn1_oid_decode (&asn1, &enterprise, &enterprise_length,
		    &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "enterprise OID", ret);
			return;
		}
		if (tree) {
			oid_string = format_oid(enterprise, enterprise_length);
#if defined(HAVE_UCD_SNMP_SNMP_H) || defined(HAVE_SNMP_SNMP_H)
# ifdef RED_HAT_MODIFIED_UCD_SNMP
			sprint_objid(binit(NULL, vb_oid_string, sizeof(vb_oid_string)), 
			    enterprise, enterprise_length);
# else
			sprint_objid(vb_oid_string, enterprise,
			    enterprise_length);
# endif
			proto_tree_add_text(tree, tvb, offset, length,
			    "Enterprise: %s (%s)", oid_string, vb_oid_string);
#else /
			proto_tree_add_text(tree, tvb, offset, length,
			    "Enterprise: %s", oid_string);
#endif /
			g_free(oid_string);
		}
		g_free(enterprise);
		offset += length;

		/
		start = asn1.offset;
		ret = asn1_header_decode (&asn1, &cls, &con, &tag,
		    &def, &agent_address_length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "agent address", ret);
			return;
		}
		if (!((cls == ASN1_APL && con == ASN1_PRI && tag == SNMP_IPA) ||
		    (cls == ASN1_UNI && con == ASN1_PRI && tag == ASN1_OTS))) {
			/
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "agent_address", ASN1_ERR_WRONG_TYPE);
			return;
		}
		if (agent_address_length != 4) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "agent_address", ASN1_ERR_WRONG_LENGTH_FOR_TYPE);
			return;
		}
		ret = asn1_string_value_decode (&asn1,
		    agent_address_length, &agent_address);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "agent address", ret);
			return;
		}
		length = asn1.offset - start;
		if (tree) {
			if (agent_address_length != 4) {
				proto_tree_add_text(tree, tvb, offset,
				    length,
				    "Agent address: <length is %u, not 4>",
				    agent_address_length);
			} else {
				proto_tree_add_text(tree, tvb, offset,
				    length,
				    "Agent address: %s",
				    ip_to_str(agent_address));
			}
		}
		g_free(agent_address);
		offset += length;
		
	        /
		ret = asn1_uint32_decode (&asn1, &trap_type, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "generic trap type", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(tree, tvb, offset, length,
			    "Trap type: %s",
			    val_to_str(trap_type, trap_types, "Unknown (%u)"));
		}		
		offset += length;
		
	        /
		ret = asn1_uint32_decode (&asn1, &specific_type, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "specific trap type", ret);
			return;
		}
		if (tree) {
			proto_tree_add_text(tree, tvb, offset, length,
			    "Specific trap type: %u (%#x)",
			    specific_type, specific_type);
		}		
		offset += length;
		
	        /
		start = asn1.offset;
		ret = asn1_header_decode (&asn1, &cls, &con, &tag,
		    &def, &timestamp_length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "timestamp", ret);
			return;
		}
		if (!((cls == ASN1_APL && con == ASN1_PRI && tag == SNMP_TIT) ||
		    (cls == ASN1_UNI && con == ASN1_PRI && tag == ASN1_INT))) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "timestamp", ASN1_ERR_WRONG_TYPE);
			return;
		}
		ret = asn1_uint32_value_decode(&asn1, timestamp_length,
		    &timestamp);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "timestamp", ret);
			return;
		}
		length = asn1.offset - start;
		if (tree) {
			proto_tree_add_text(tree, tvb, offset, length,
			    "Timestamp: %u", timestamp);
		}		
		offset += length;
		break;
	}

	/
	/
	ret = asn1_sequence_decode(&asn1, &variable_bindings_length, &length);
	if (ret != ASN1_ERR_NOERROR) {
		dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			"variable bindings header", ret);
		return;
	}
	offset += length;

	/
	vb_index = 0;
	while (variable_bindings_length > 0) {
		vb_index++;
		sequence_length = 0;

		/
		ret = asn1_sequence_decode(&asn1, &variable_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
				"variable binding header", ret);
			return;
		}
		sequence_length += length;

		/
		ret = asn1_oid_decode (&asn1, &variable_oid,
		    &variable_oid_length, &length);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "variable binding OID", ret);
			return;
		}
		sequence_length += length;

		unsafe = FALSE;
		if (tree) {
			oid_string = format_oid(variable_oid,
			    variable_oid_length);
			
#if defined(HAVE_UCD_SNMP_SNMP_H) || defined(HAVE_SNMP_SNMP_H)
# ifdef RED_HAT_MODIFIED_UCD_SNMP
			sprint_objid(binit(NULL, vb_oid_string, sizeof(vb_oid_string)), 
			    variable_oid, variable_oid_length);
# else
			sprint_objid(vb_oid_string, variable_oid,
			    variable_oid_length);
# endif
			proto_tree_add_text(tree, tvb, offset, sequence_length,
			    "Object identifier %d: %s (%s)", vb_index,
			    oid_string, vb_oid_string);
#ifdef HAVE_SNMP_SNMP_H
			/
			if (vb_oid_string[0] == '.' &&
			    isdigit((guchar)vb_oid_string[1]))
				unsafe = TRUE;
#endif /
#else /
			proto_tree_add_text(tree, tvb, offset, sequence_length,
			    "Object identifier %d: %s", vb_index,
			    oid_string);
#endif /
			g_free(oid_string);
		}
		offset += sequence_length;
		variable_bindings_length -= sequence_length;
				
		/
		ret = snmp_variable_decode(tree, variable_oid,
		    variable_oid_length, &asn1, offset, &length,
		    unsafe);
		if (ret != ASN1_ERR_NOERROR) {
			dissect_snmp_parse_error(tvb, offset, pinfo, tree,
			    "variable", ret);
			return;
		}
		offset += length;
		variable_bindings_length -= length;
	}
}
