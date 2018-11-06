void
proto_item_fill_label(field_info *fi, gchar *label_str)
{
	struct header_field_info	*hfinfo = fi->hfinfo;
	guint32				n_addr; /

	switch(hfinfo->type) {
		case FT_NONE:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s", hfinfo->name);
			break;

		case FT_BOOLEAN:
			fill_label_boolean(fi, label_str);
			break;

		case FT_BYTES:
			if (fi->value.bytes) {
				snprintf(label_str, ITEM_LABEL_LENGTH,
					"%s: %s", hfinfo->name, 
					 bytes_to_str(fi->value.bytes, fi->length));
			}
			else {
				snprintf(label_str, ITEM_LABEL_LENGTH,
					"%s: <MISSING>", hfinfo->name);
			}
			break;

		/
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
			if (hfinfo->bitmask) {
				if (hfinfo->strings) {
					fill_label_enumerated_bitfield(fi, label_str);
				}
				else {
					fill_label_numeric_bitfield(fi, label_str);
				}
			}
			else {
				if (hfinfo->strings) {
					fill_label_enumerated_uint(fi, label_str);
				}
				else {
					fill_label_uint(fi, label_str);
				}
			}
			break;

		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			g_assert(!hfinfo->bitmask);
			if (hfinfo->strings) {
				fill_label_enumerated_int(fi, label_str);
			}
			else {
				fill_label_int(fi, label_str);
			}
			break;

		case FT_DOUBLE:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %g", fi->hfinfo->name,
				fi->value.floating);
			break;

		case FT_ABSOLUTE_TIME:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s", fi->hfinfo->name,
				abs_time_to_str(&fi->value.time));
			break;

		case FT_RELATIVE_TIME:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s seconds", fi->hfinfo->name,
				rel_time_to_str(&fi->value.time));
			break;

		case FT_IPXNET:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: 0x%08X (%s)", fi->hfinfo->name,
				fi->value.numeric, get_ipxnet_name(fi->value.numeric));
			break;

		case FT_ETHER:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s (%s)", fi->hfinfo->name,
				ether_to_str(fi->value.ether),
				get_ether_name(fi->value.ether));
			break;

		case FT_IPv4:
			n_addr = ipv4_get_net_order_addr(&fi->value.ipv4);
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s (%s)", fi->hfinfo->name,
				get_hostname(n_addr),
				ip_to_str((guint8*)&n_addr));
			break;

		case FT_IPv6:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s (%s)", fi->hfinfo->name,
				get_hostname6((struct e_in6_addr *)fi->value.ipv6),
				ip6_to_str((struct e_in6_addr*)fi->value.ipv6));
			break;
	
		case FT_STRING:
		case FT_NSTRING_UINT8:
			snprintf(label_str, ITEM_LABEL_LENGTH,
				"%s: %s", fi->hfinfo->name, fi->value.string);
			break;

		default:
			g_error("hfinfo->type %d not handled\n", fi->hfinfo->type);
			break;
	}
}
