const gchar *
proto_custom_set(proto_tree* tree, const int field_id, gint occurrence,
                 gchar *result, gchar *expr, const int size)
{
	guint32		u_integer;
	gint32		integer;
	guint8		*bytes;
	ipv4_addr	*ipv4;
	struct e_in6_addr *ipv6;
	address		addr;
	guint32		n_addr; /

	const true_false_string  *tfstring;
	int		len, last, i, offset=0;
	GPtrArray	*finfos;
	field_info	*finfo = NULL;
	header_field_info* hfinfo;

	g_assert(field_id >= 0);

	hfinfo = proto_registrar_get_nth((guint)field_id);

	/
	if (!hfinfo)
		return "";

	while (hfinfo) {
		finfos = proto_get_finfo_ptr_array(tree, hfinfo->id);

		if (!finfos || !(len = g_ptr_array_len(finfos))) {
			hfinfo = hfinfo->same_name_next;
			continue;
		}

                /
                if ((occurrence > len) || (occurrence < -len) )
                        return "";

                /
                if (occurrence < 0) {
                        i = occurrence + len;
                        last = i;
                } else if (occurrence > 0) {
                        i = occurrence - 1;
                        last = i;
                } else {
                        i = 0;
                        last = len - 1;
                }

                while (i <= last) {
                        finfo = g_ptr_array_index(finfos, i);

                        if (offset && (offset < size-2))
                                result[offset++]=',';

                        switch(hfinfo->type) {

                        case FT_NONE: /
                                result[0] = '\0';
                                break;

                        case FT_PROTOCOL:
                                /
                                g_strlcpy(result, "Yes", size);
                                break;

                        case FT_UINT_BYTES:
                        case FT_BYTES:
                                bytes = fvalue_get(&finfo->value);
                                offset += (int)g_strlcpy(result+offset, bytes_to_str(bytes, fvalue_length(&finfo->value)), size-offset);
                                break;

                        case FT_ABSOLUTE_TIME:
                                offset += (int)g_strlcpy(result+offset,
                                        abs_time_to_str(fvalue_get(&finfo->value), hfinfo->display, TRUE),
                                        size-offset);
                                break;

                        case FT_RELATIVE_TIME:
                                offset += (int)g_strlcpy(result+offset, rel_time_to_secs_str(fvalue_get(&finfo->value)), size-offset);
                                break;

                        case FT_BOOLEAN:
                                u_integer = fvalue_get_uinteger(&finfo->value);
                                tfstring = (const true_false_string *)&tfs_true_false;
                                if (hfinfo->strings) {
                                        tfstring = (const struct true_false_string*) hfinfo->strings;
                                }
                                offset += (int)g_strlcpy(result+offset, u_integer ? tfstring->true_string : tfstring->false_string, size-offset);
                                break;

                        case FT_UINT8:
                        case FT_UINT16:
                        case FT_UINT24:
                        case FT_UINT32:
                        case FT_FRAMENUM:
                                u_integer = fvalue_get_uinteger(&finfo->value);
                                if (hfinfo->strings) {
                                        if (hfinfo->display & BASE_RANGE_STRING) {
                                                offset += (int)g_strlcpy(result+offset, rval_to_str(u_integer, hfinfo->strings, "%u"), size-offset);
                                        } else if (hfinfo->display & BASE_EXT_STRING) {
                                                offset += (int)g_strlcpy(result+offset, val_to_str_ext(u_integer, (value_string_ext *) (hfinfo->strings), "%u"), size-offset);
                                        } else {
                                                offset += (int)g_strlcpy(result+offset, val_to_str(u_integer, cVALS(hfinfo->strings), "%u"), size-offset);
                                        }
                                } else if (IS_BASE_DUAL(hfinfo->display)) {
                                        g_snprintf(result+offset, size-offset, hfinfo_uint_value_format(hfinfo), u_integer, u_integer);
                                        offset = (int)strlen(result);
                                } else {
                                        g_snprintf(result+offset, size-offset, hfinfo_uint_value_format(hfinfo), u_integer);
                                        offset = (int)strlen(result);
                                }
                                break;

                        case FT_INT64:
                        case FT_UINT64:
                                g_snprintf(result+offset, size-offset, "%" G_GINT64_MODIFIER "u", fvalue_get_integer64(&finfo->value));
                                offset = (int)strlen(result);
                                break;

                        /
                        case FT_INT8:
                        case FT_INT16:
                        case FT_INT24:
                        case FT_INT32:
                                integer = fvalue_get_sinteger(&finfo->value);
                                if (hfinfo->strings) {
                                        if (hfinfo->display & BASE_RANGE_STRING) {
                                                offset += (int)g_strlcpy(result+offset, rval_to_str(integer, hfinfo->strings, "%d"), size-offset);
                                        } else if (hfinfo->display & BASE_EXT_STRING) {
                                                offset += (int)g_strlcpy(result+offset, val_to_str_ext(integer, (value_string_ext *) (hfinfo->strings), "%d"), size-offset);
                                        } else {
                                                offset += (int)g_strlcpy(result+offset, val_to_str(integer, cVALS(hfinfo->strings), "%d"), size-offset);
                                        }
                                } else if (IS_BASE_DUAL(hfinfo->display)) {
                                        g_snprintf(result+offset, size-offset, hfinfo_int_value_format(hfinfo), integer, integer);
                                        offset = (int)strlen(result);
                                } else {
                                        g_snprintf(result+offset, size-offset, hfinfo_int_value_format(hfinfo), integer);
                                        offset = (int)strlen(result);
                                }
                                break;

                        case FT_IPv4:
                                ipv4 = fvalue_get(&finfo->value);
                                n_addr = ipv4_get_net_order_addr(ipv4);
                                offset += (int)g_strlcpy(result+offset, ip_to_str((guint8 *)&n_addr), size-offset);
                                break;

                        case FT_IPv6:
                                ipv6 = fvalue_get(&finfo->value);
                                SET_ADDRESS (&addr, AT_IPv6, sizeof(struct e_in6_addr), ipv6);
                                address_to_str_buf(&addr, result+offset, size-offset);
                                offset = (int)strlen(result);
                                break;

                        case FT_ETHER:
                                offset += (int)g_strlcpy(result+offset, bytes_to_str_punct(fvalue_get(&finfo->value), 6, ':'), size-offset);
                                break;

                        case FT_GUID:
                                offset += (int)g_strlcpy(result+offset, guid_to_str((e_guid_t *)fvalue_get(&finfo->value)), size-offset);
                                break;

                        case FT_OID:
                                bytes = fvalue_get(&finfo->value);
                                offset += (int)g_strlcpy(result+offset, oid_resolved_from_encoded(bytes, fvalue_length(&finfo->value)), size-offset);
                                break;

                        case FT_FLOAT:
                                g_snprintf(result+offset, size-offset, "%." STRINGIFY(FLT_DIG) "f", fvalue_get_floating(&finfo->value));
                                offset = (int)strlen(result);
                                break;

                        case FT_DOUBLE:
                                g_snprintf(result+offset, size-offset, "%." STRINGIFY(DBL_DIG) "g", fvalue_get_floating(&finfo->value));
                                offset = (int)strlen(result);
                                break;

                        case FT_EBCDIC:
                        case FT_STRING:
                        case FT_STRINGZ:
                        case FT_UINT_STRING:
                                bytes = fvalue_get(&finfo->value);
                                offset += (int)g_strlcpy(result+offset, format_text(bytes, strlen(bytes)), size-offset);
                                break;

                        case FT_IPXNET: /
                        case FT_PCRE:
                        default:
                                g_error("hfinfo->type %d (%s) not handled\n",
                                                hfinfo->type,
                                                ftype_name(hfinfo->type));
                                DISSECTOR_ASSERT_NOT_REACHED();
                                break;
                        }
                        i++;
                }

                if(occurrence) {
                        switch(hfinfo->type) {

                        case FT_BOOLEAN:
                                g_snprintf(expr, size, "%u", fvalue_get_uinteger(&finfo->value) ? 1 : 0);
                                break;

                        case FT_UINT8:
                        case FT_UINT16:
                        case FT_UINT24:
                        case FT_UINT32:
                        case FT_FRAMENUM:
                                g_snprintf(expr, size, hfinfo_numeric_value_format(hfinfo), fvalue_get_uinteger(&finfo->value));
                                break;

                        case FT_INT8:
                        case FT_INT16:
                        case FT_INT24:
                        case FT_INT32:
                                g_snprintf(expr, size, hfinfo_numeric_value_format(hfinfo), fvalue_get_sinteger(&finfo->value));
                                break;

                        case FT_OID:
                                bytes = fvalue_get(&finfo->value);
                                g_strlcpy(expr, oid_encoded2string(bytes, fvalue_length(&finfo->value)), size);
                                break;

                        default:
                                g_strlcpy(expr, result, size);
                                break;
                        }
                }
                /

		return hfinfo->abbrev;
	}
	return "";
}
