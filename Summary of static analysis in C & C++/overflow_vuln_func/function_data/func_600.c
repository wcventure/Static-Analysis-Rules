const gchar *
proto_custom_set(proto_tree* tree, const int field_id, gint occurrence,
		 gchar *result, gchar *expr, const int size)
{
	guint32            u_integer;
	gint32             integer;
	guint8            *bytes;
	ipv4_addr         *ipv4;
	struct e_in6_addr *ipv6;
	address            addr;
	guint32            n_addr; /

	const true_false_string  *tfstring;

	int                 len, prev_len = 0, last, i, offset_r = 0, offset_e = 0;
	GPtrArray          *finfos;
	field_info         *finfo         = NULL;
	header_field_info*  hfinfo;
	const gchar        *abbrev        = NULL;

	g_assert(field_id >= 0);

	hfinfo = proto_registrar_get_nth((guint)field_id);

	/
	if (!hfinfo)
		return "";

	if (occurrence < 0) {
		/
		while (hfinfo->same_name_prev) {
			hfinfo = hfinfo->same_name_prev;
		}
	}

	while (hfinfo) {
		finfos = proto_get_finfo_ptr_array(tree, hfinfo->id);

		if (!finfos || !(len = g_ptr_array_len(finfos))) {
			if (occurrence < 0) {
				hfinfo = hfinfo->same_name_next;
			} else {
				hfinfo = hfinfo->same_name_prev;
			}
			continue;
		}

		/
		if (((occurrence - prev_len) > len) || ((occurrence + prev_len) < -len)) {
			if (occurrence < 0) {
				hfinfo = hfinfo->same_name_next;
			} else {
				hfinfo = hfinfo->same_name_prev;
			}
			prev_len += len;
			continue;
		}

		/
		if (occurrence < 0) {
			i = occurrence + len + prev_len;
			last = i;
		} else if (occurrence > 0) {
			i = occurrence - 1 - prev_len;
			last = i;
		} else {
			i = 0;
			last = len - 1;
		}

		prev_len += len; /

		while (i <= last) {
			finfo = g_ptr_array_index(finfos, i);

			if (offset_r && (offset_r < (size - 2)))
				result[offset_r++] = ',';

			if (offset_e && (offset_e < (size - 2)))
				expr[offset_e++] = ',';

			switch (hfinfo->type) {

			case FT_NONE: /
				if (offset_r == 0) {
					result[0] = '\0';
				} else if (result[offset_r-1] == ',') {
					result[offset_r-1] = '\0';
				}
				break;

			case FT_PROTOCOL:
				/
				g_strlcpy(result, "Yes", size);
				break;

			case FT_UINT_BYTES:
			case FT_BYTES:
				bytes = fvalue_get(&finfo->value);
				offset_r += (int)g_strlcpy(result+offset_r,
							   bytes_to_str(bytes,
									fvalue_length(&finfo->value)),
							   size-offset_r);
				break;

			case FT_ABSOLUTE_TIME:
				offset_r += (int)g_strlcpy(result+offset_r,
							   abs_time_to_str(fvalue_get(&finfo->value),
									   hfinfo->display, TRUE),
							   size-offset_r);
				break;

			case FT_RELATIVE_TIME:
				offset_r += (int)g_strlcpy(result+offset_r,
							   rel_time_to_secs_str(fvalue_get(&finfo->value)),
							   size-offset_r);
				break;

			case FT_BOOLEAN:
				u_integer = fvalue_get_uinteger(&finfo->value);
				tfstring = (const true_false_string *)&tfs_true_false;
				if (hfinfo->strings) {
					tfstring = (const struct true_false_string*) hfinfo->strings;
				}
				offset_r += (int)g_strlcpy(result+offset_r,
							   u_integer ?
							     tfstring->true_string :
							     tfstring->false_string, size-offset_r);

				g_snprintf(expr+offset_e, size-offset_e, "%u",
					   fvalue_get_uinteger(&finfo->value) ? 1 : 0);
				offset_e = (int)strlen(expr);
				break;

			case FT_UINT8:
			case FT_UINT16:
			case FT_UINT24:
			case FT_UINT32:
			case FT_FRAMENUM:
				u_integer = fvalue_get_uinteger(&finfo->value);
				if (hfinfo->strings) {
					if (hfinfo->display & BASE_CUSTOM) {
						g_snprintf(result+offset_r, size-offset_r, "%u", u_integer);
					} else if (hfinfo->display & BASE_RANGE_STRING) {
						g_strlcpy(result+offset_r,
							  rval_to_str(u_integer, hfinfo->strings, "%u"),
							  size-offset_r);
					} else if (hfinfo->display & BASE_EXT_STRING) {
						g_strlcpy(result+offset_r,
							  val_to_str_ext(u_integer,
									 (value_string_ext *)(hfinfo->strings),
									 "%u"), size-offset_r);
					} else {
						g_strlcpy(result+offset_r,
							  val_to_str(u_integer, cVALS(hfinfo->strings), "%u"),
							  size-offset_r);
					}
				} else if (IS_BASE_DUAL(hfinfo->display)) {
					g_snprintf(result+offset_r, size-offset_r,
						   hfinfo_uint_value_format(hfinfo), u_integer, u_integer);
				} else {
					g_snprintf(result+offset_r, size-offset_r,
						   hfinfo_uint_value_format(hfinfo), u_integer);
				}

				if (hfinfo->strings && (hfinfo->display & BASE_DISPLAY_E_MASK) == BASE_NONE) {
					g_snprintf(expr+offset_e, size-offset_e,
						   "\"%s\"", result+offset_r);
				} else {
					g_snprintf(expr+offset_e, size-offset_e,
						   hfinfo_numeric_value_format(hfinfo),
						   fvalue_get_uinteger(&finfo->value));
				}

				offset_r = (int)strlen(result);
				offset_e = (int)strlen(expr);
				break;

			case FT_INT64:
				g_snprintf(result+offset_r, size-offset_r,
					   "%" G_GINT64_MODIFIER "d",
					   fvalue_get_integer64(&finfo->value));
				offset_r = (int)strlen(result);
				break;
			case FT_UINT64:
				g_snprintf(result+offset_r, size-offset_r,
					   "%" G_GINT64_MODIFIER "u",
					   fvalue_get_integer64(&finfo->value));
				offset_r = (int)strlen(result);
				break;
			case FT_EUI64:
				offset_r += (int)g_strlcpy(result+offset_r,
							   eui64_to_str(fvalue_get_integer64(&finfo->value)),
							   size-offset_r);
				break;
			/
			case FT_INT8:
			case FT_INT16:
			case FT_INT24:
			case FT_INT32:
				integer = fvalue_get_sinteger(&finfo->value);
				if (hfinfo->strings) {
					if (hfinfo->display & BASE_CUSTOM) {
						g_snprintf(result+offset_r, size-offset_r, "%d", integer);
					} else if (hfinfo->display & BASE_RANGE_STRING) {
						g_strlcpy(result+offset_r,
							  rval_to_str(integer, hfinfo->strings, "%d"),
							  size-offset_r);
					} else if (hfinfo->display & BASE_EXT_STRING) {
						g_strlcpy(result+offset_r,
							  val_to_str_ext(integer,
									 (value_string_ext *)(hfinfo->strings),
									 "%d"),
							  size-offset_r);
					} else {
						g_strlcpy(result+offset_r,
							  val_to_str(integer, cVALS(hfinfo->strings), "%d"),
							  size-offset_r);
					}
				} else if (IS_BASE_DUAL(hfinfo->display)) {
					g_snprintf(result+offset_r, size-offset_r,
						   hfinfo_int_value_format(hfinfo), integer, integer);
				} else {
					g_snprintf(result+offset_r, size-offset_r,
						   hfinfo_int_value_format(hfinfo), integer);
				}

				if (hfinfo->strings && (hfinfo->display & BASE_DISPLAY_E_MASK) == BASE_NONE) {
					g_snprintf(expr+offset_e, size-offset_e, "\"%s\"", result+offset_r);
				} else {
					g_snprintf(expr+offset_e, size-offset_e,
						   hfinfo_numeric_value_format(hfinfo),
						   fvalue_get_sinteger(&finfo->value));
				}

				offset_r = (int)strlen(result);
				offset_e = (int)strlen(expr);
				break;

			case FT_IPv4:
				ipv4 = fvalue_get(&finfo->value);
				n_addr = ipv4_get_net_order_addr(ipv4);
				offset_r += (int)g_strlcpy(result+offset_r,
							   ip_to_str((guint8 *)&n_addr),
							   size-offset_r);
				break;

			case FT_IPv6:
				ipv6 = fvalue_get(&finfo->value);
				SET_ADDRESS (&addr, AT_IPv6, sizeof(struct e_in6_addr), ipv6);
				address_to_str_buf(&addr, result+offset_r, size-offset_r);
				offset_r = (int)strlen(result);
				break;

			case FT_ETHER:
				offset_r += (int)g_strlcpy(result+offset_r,
							   bytes_to_str_punct(fvalue_get(&finfo->value),
									      FT_ETHER_LEN, ':'),
							   size-offset_r);
				break;

			case FT_GUID:
				offset_r += (int)g_strlcpy(result+offset_r,
							   guid_to_str((e_guid_t *)fvalue_get(&finfo->value)),
							   size-offset_r);
				break;

			case FT_OID:
				bytes = fvalue_get(&finfo->value);
				offset_r += (int)g_strlcpy(result+offset_r,
							   oid_resolved_from_encoded(bytes,
										     fvalue_length(&finfo->value)),
							   size-offset_r);
				offset_e += (int)g_strlcpy(expr+offset_e,
							   oid_encoded2string(bytes, fvalue_length(&finfo->value)),
							   size-offset_e);
				break;

			case FT_FLOAT:
				g_snprintf(result+offset_r, size-offset_r,
					   "%." STRINGIFY(FLT_DIG) "g", fvalue_get_floating(&finfo->value));
				offset_r = (int)strlen(result);
				break;

			case FT_DOUBLE:
				g_snprintf(result+offset_r, size-offset_r,
					   "%." STRINGIFY(DBL_DIG) "g", fvalue_get_floating(&finfo->value));
				offset_r = (int)strlen(result);
				break;

			case FT_STRING:
			case FT_STRINGZ:
			case FT_UINT_STRING:
				bytes = fvalue_get(&finfo->value);
				offset_r += (int)g_strlcpy(result+offset_r,
							   format_text(bytes, strlen(bytes)),
							   size-offset_r);
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

		switch (hfinfo->type) {

		case FT_BOOLEAN:
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
		case FT_FRAMENUM:
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
		case FT_OID:
			/
			break;

		default:
			/
			g_strlcpy(expr, result, size);
			break;
		}

		if (!abbrev) {
			/
			abbrev = hfinfo->abbrev;
		}

		if (occurrence == 0) {
			/
			hfinfo = hfinfo->same_name_prev;
		} else {
			hfinfo = NULL;
		}
	}

	return abbrev ? abbrev : "";
}
