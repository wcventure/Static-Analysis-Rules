static int
snmp_variable_decode(proto_tree *snmp_tree, subid_t *variable_oid,
    guint variable_oid_length, ASN1_SCK *asn1, int offset, guint *lengthp,
    gboolean unsafe)
{
	int start;
	guint length;
	gboolean def;
	guint vb_length;
	gushort vb_type;
	gchar *vb_type_name;
	int ret;
	guint cls, con, tag;

	gint32 vb_integer_value;
	guint32 vb_uinteger_value;

	guint8 *vb_octet_string;

	subid_t *vb_oid;
	guint vb_oid_length;

	gchar *vb_display_string;

#ifdef HAVE_SPRINT_VALUE
	struct variable_list variable;
#if defined(HAVE_UCD_SNMP_SNMP_H)
	long value;
#endif
#endif	/
	unsigned int i;
	gchar *buf;
	int len;

	/
	start = asn1->offset;
	ret = asn1_header_decode (asn1, &cls, &con, &tag, &def, &vb_length);
	if (ret != ASN1_ERR_NOERROR)
		return ret;
	if (!def)
		return ASN1_ERR_LENGTH_NOT_DEFINITE;

	/
	vb_type_name = snmp_tag_cls2syntax(tag, cls, &vb_type);
	if (vb_type_name == NULL) {
		/
		vb_type_name = "unsupported type";
		vb_type = SNMP_OPAQUE;
	}

	/
	switch (vb_type) {

	case SNMP_INTEGER:
		ret = asn1_int32_value_decode(asn1, vb_length,
		    &vb_integer_value);
		if (ret != ASN1_ERR_NOERROR)
			return ret;
		length = asn1->offset - start;
		if (snmp_tree) {
#ifdef HAVE_SPRINT_VALUE
			if (!unsafe) {
#if defined(HAVE_UCD_SNMP_SNMP_H)
				value = vb_integer_value;
				variable.val.integer = &value;
#elif defined(HAVE_SNMP_SNMP_H)
				variable.val.integer = &vb_integer_value;
#endif
				vb_display_string = format_var(&variable,
				    variable_oid, variable_oid_length, vb_type,
				    vb_length);
				proto_tree_add_text(snmp_tree, asn1->tvb, offset,
				    length,
				    "Value: %s", vb_display_string);
				g_free(vb_display_string);
				break;	/
			}
#endif /
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: %d (%#x)", vb_type_name,
			    vb_integer_value, vb_integer_value);
		}
		break;

	case SNMP_COUNTER:
	case SNMP_GAUGE:
	case SNMP_TIMETICKS:
		ret = asn1_uint32_value_decode(asn1, vb_length,
		    &vb_uinteger_value);
		if (ret != ASN1_ERR_NOERROR)
			return ret;
		length = asn1->offset - start;
		if (snmp_tree) {
#ifdef HAVE_SPRINT_VALUE
			if (!unsafe) {
#if defined(HAVE_UCD_SNMP_SNMP_H)
				value = vb_uinteger_value;
				variable.val.integer = &value;
#elif defined(HAVE_SNMP_SNMP_H)
				variable.val.integer = &vb_uinteger_value;
#endif
				vb_display_string = format_var(&variable,
				    variable_oid, variable_oid_length, vb_type,
				    vb_length);
				proto_tree_add_text(snmp_tree, asn1->tvb, offset,
				    length,
				    "Value: %s", vb_display_string);
				g_free(vb_display_string);
				break;	/
			}
#endif /
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: %u (%#x)", vb_type_name,
			    vb_uinteger_value, vb_uinteger_value);
		}
		break;

	case SNMP_OCTETSTR:
	case SNMP_IPADDR:
	case SNMP_OPAQUE:
	case SNMP_NSAP:
	case SNMP_BITSTR:
	case SNMP_COUNTER64:
		ret = asn1_string_value_decode (asn1, vb_length,
		    &vb_octet_string);
		if (ret != ASN1_ERR_NOERROR)
			return ret;
		length = asn1->offset - start;
		if (snmp_tree) {
#ifdef HAVE_SPRINT_VALUE
			if (!unsafe) {
				variable.val.string = vb_octet_string;
				vb_display_string = format_var(&variable,
				    variable_oid, variable_oid_length, vb_type,
				    vb_length);
				proto_tree_add_text(snmp_tree, asn1->tvb, offset,
				    length,
				    "Value: %s", vb_display_string);
				g_free(vb_display_string);
				break;	/
			}
#endif /
			/
			for (i = 0; i < vb_length; i++) {
				if (!(isprint(vb_octet_string[i])
				    || isspace(vb_octet_string[i])))
					break;
			}
			if (i < vb_length) {
				/
				vb_display_string = g_malloc(4*vb_length);
				buf = &vb_display_string[0];
				len = sprintf(buf, "%03u", vb_octet_string[0]);
				buf += len;
				for (i = 1; i < vb_length; i++) {
					len = sprintf(buf, ".%03u",
					    vb_octet_string[i]);
					buf += len;
				}
				proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
				    "Value: %s: %s", vb_type_name,
				    vb_display_string);
				g_free(vb_display_string);
			} else {
				proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
				    "Value: %s: %.*s", vb_type_name,
				    (int)vb_length,
				    SAFE_STRING(vb_octet_string));
			}
		}
		g_free(vb_octet_string);
		break;

	case SNMP_NULL:
		ret = asn1_null_decode (asn1, vb_length);
		if (ret != ASN1_ERR_NOERROR)
			return ret;
		length = asn1->offset - start;
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s", vb_type_name);
		}
		break;

	case SNMP_OBJECTID:
		ret = asn1_oid_value_decode (asn1, vb_length, &vb_oid,
		    &vb_oid_length);
		if (ret != ASN1_ERR_NOERROR)
			return ret;
		length = asn1->offset - start;
		if (snmp_tree) {
#ifdef HAVE_SPRINT_VALUE
			if (!unsafe) {
				variable.val.objid = vb_oid;
				vb_display_string = format_var(&variable,
				    variable_oid, variable_oid_length, vb_type,
				    vb_length);
				proto_tree_add_text(snmp_tree, asn1->tvb, offset,
				    length,
				    "Value: %s", vb_display_string);
				break;	/
			}
#endif /
			vb_display_string = format_oid(vb_oid, vb_oid_length);
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: %s", vb_type_name, vb_display_string);
			g_free(vb_display_string);
		}
		g_free(vb_oid);
		break;

	case SNMP_NOSUCHOBJECT:
		length = asn1->offset - start;
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: no such object", vb_type_name);
		}
		break;

	case SNMP_NOSUCHINSTANCE:
		length = asn1->offset - start;
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: no such instance", vb_type_name);
		}
		break;

	case SNMP_ENDOFMIBVIEW:
		length = asn1->offset - start;
		if (snmp_tree) {
			proto_tree_add_text(snmp_tree, asn1->tvb, offset, length,
			    "Value: %s: end of mib view", vb_type_name);
		}
		break;

	default:
		g_assert_not_reached();
		return ASN1_ERR_WRONG_TYPE;
	}
	*lengthp = length;
	return ASN1_ERR_NOERROR;
}
