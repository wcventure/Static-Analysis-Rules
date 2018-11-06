proto_item *
proto_tree_add_item(proto_tree *tree, int hfindex, tvbuff_t *tvb,
    gint start, gint length, gboolean little_endian)
{
	field_info	*new_fi;
	proto_item	*pi;
	guint32		value, n;

	new_fi = alloc_field_info(hfindex, tvb, start, length);

	if (new_fi == NULL)
		return(NULL);

	switch(new_fi->hfinfo->type) {
		case FT_NONE:
			/
			break;

		case FT_BYTES:
			proto_tree_set_bytes_tvb(new_fi, tvb, start, length);
			break;

		case FT_BOOLEAN:
			proto_tree_set_boolean(new_fi,
			    get_uint_value(tvb, start, length, little_endian));
			break;

		/
		case FT_UINT8:
		case FT_UINT16:
		case FT_UINT24:
		case FT_UINT32:
			proto_tree_set_uint(new_fi,
			    get_uint_value(tvb, start, length, little_endian));
			break;

		/
		case FT_INT8:
		case FT_INT16:
		case FT_INT24:
		case FT_INT32:
			proto_tree_set_int(new_fi,
			    get_int_value(tvb, start, length, little_endian));
			break;

		case FT_IPv4:
			g_assert(length == 4);
			tvb_memcpy(tvb, (guint8 *)&value, start, 4);
			proto_tree_set_ipv4(new_fi, value);
			break;

		case FT_IPXNET:
			g_assert(length == 4);
			proto_tree_set_ipxnet(new_fi,
			    get_uint_value(tvb, start, 4, FALSE));
			break;

		case FT_IPv6:
			g_assert(length == 16);
			proto_tree_set_ipv6_tvb(new_fi, tvb, start);
			break;

		case FT_ETHER:
			g_assert(length == 6);
			proto_tree_set_ether_tvb(new_fi, tvb, start);
			break;

		case FT_STRING:
			/
			proto_tree_set_string_tvb(new_fi, tvb, start, length);
			break;

		case FT_NSTRING_UINT8:
			n = tvb_get_guint8(tvb, start);
			/
			proto_tree_set_string_tvb(new_fi, tvb, start + 1, n);
			/
			new_fi->length = n + 1;
			break;

		default:
			g_error("new_fi->hfinfo->type %d not handled\n", new_fi->hfinfo->type);
			g_assert_not_reached();
			break;
	}

	/
	pi = proto_tree_add_node(tree, new_fi);

	return pi;
}
