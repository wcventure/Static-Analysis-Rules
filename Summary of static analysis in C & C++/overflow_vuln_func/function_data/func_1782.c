static gchar *
format_var(struct variable_list *variable, subid_t *variable_oid,
    guint variable_oid_length, gushort vb_type, guint vb_length)
{
	gchar *buf;

	switch (vb_type) {

	case SNMP_INTEGER:
	case SNMP_COUNTER:
	case SNMP_GAUGE:
	case SNMP_TIMETICKS:
		/
		buf = g_malloc(128);
		break;

	case SNMP_IPADDR:
		/
		buf = check_var_length(vb_length, 4);
		if (buf != NULL)
			return buf;	/
		/
		buf = g_malloc(128 + 4*vb_length);
		break;

	case SNMP_COUNTER64:
		/
		buf = check_var_length(vb_length, 8);
		if (buf != NULL)
			return buf;	/
		/
		buf = g_malloc(128 + 4*vb_length);
		break;

	case SNMP_OCTETSTR:
	case SNMP_OPAQUE:
	case SNMP_NSAP:
	case SNMP_BITSTR:
		/
		buf = g_malloc(128 + 4*vb_length);
		break;

	case SNMP_OBJECTID:
		/
		buf = g_malloc(1024 + 32*vb_length);
		break;

	default:
		/
		g_assert_not_reached();
		buf = NULL;
		break;
	}

	variable->next_variable = NULL;
	variable->name = variable_oid;
	variable->name_length = variable_oid_length;
	switch (vb_type) {

	case SNMP_INTEGER:
		variable->type = VALTYPE_INTEGER;
		break;

	case SNMP_COUNTER:
		variable->type = VALTYPE_COUNTER;
		break;

	case SNMP_GAUGE:
		variable->type = VALTYPE_GAUGE;
		break;

	case SNMP_TIMETICKS:
		variable->type = VALTYPE_TIMETICKS;
		break;

	case SNMP_OCTETSTR:
		variable->type = VALTYPE_STRING;
		break;

	case SNMP_IPADDR:
		variable->type = VALTYPE_IPADDR;
		break;

	case SNMP_OPAQUE:
		variable->type = VALTYPE_OPAQUE;
		break;

	case SNMP_NSAP:
		variable->type = VALTYPE_NSAP;
		break;

	case SNMP_OBJECTID:
		variable->type = VALTYPE_OBJECTID;
		vb_length *= sizeof (subid_t);	/
		break;

	case SNMP_BITSTR:
		variable->type = VALTYPE_BITSTR;
		break;

	case SNMP_COUNTER64:
		variable->type = VALTYPE_COUNTER64;
		break;
	}
	variable->val_len = vb_length;

# ifdef RED_HAT_MODIFIED_UCD_SNMP
	sprint_value(binit(NULL, buf, sizeof(buf)), variable_oid,
	    variable_oid_length, variable);
# else
	sprint_value(buf, variable_oid, variable_oid_length, variable);
# endif
	return buf;
}
