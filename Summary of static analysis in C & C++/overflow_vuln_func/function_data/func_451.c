gint
proto_registrar_get_length(int n)
{
	struct header_field_info *hfinfo;

	hfinfo = proto_registrar_get_nth(n);
	if (!hfinfo)
		return -1;

	switch (hfinfo->type) {
		case FT_TEXT_ONLY: /
		case NUM_FIELD_TYPES: /
			return -1;

		case FT_NONE:
		case FT_BYTES:
		case FT_BOOLEAN:
		case FT_STRING:
		case FT_NSTRING_UINT8:
		case FT_DOUBLE:
		case FT_ABSOLUTE_TIME:
		case FT_RELATIVE_TIME:
			return 0;

		case FT_UINT8:
		case FT_INT8:
			return 1;

		case FT_UINT16:
		case FT_INT16:
			return 2;

		case FT_UINT24:
		case FT_INT24:
			return 3;

		case FT_UINT32:
		case FT_INT32:
		case FT_IPXNET:
		case FT_IPv4:
			return 4;

		case FT_ETHER:
			return 6;

		case FT_IPv6:
			return 16;
	}
	g_assert_not_reached();
	return -1;
}
