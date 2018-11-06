void
proto_registrar_dump(void)
{
	header_field_info	*hfinfo, *parent_hfinfo;
	int			i, len;
	const char 		*enum_name;

	len = gpa_hfinfo->len;
	for (i = 0; i < len ; i++) {
		hfinfo = proto_registrar_get_nth(i);

		/
		if (proto_registrar_is_protocol(i)) {
			printf("P\t%s\t%s\n", hfinfo->name, hfinfo->abbrev);
		}
		/
		else {
			parent_hfinfo = proto_registrar_get_nth(hfinfo->parent);
			g_assert(parent_hfinfo);

			switch(hfinfo->type) {
			case FT_NONE:
				enum_name = "FT_NONE";
				break;
			case FT_BOOLEAN:
				enum_name = "FT_BOOLEAN";
				break;
			case FT_UINT8:
				enum_name = "FT_UINT8";
				break;
			case FT_UINT16:
				enum_name = "FT_UINT16";
				break;
			case FT_UINT24:
				enum_name = "FT_UINT24";
				break;
			case FT_UINT32:
				enum_name = "FT_UINT32";
				break;
			case FT_INT8:
				enum_name = "FT_INT8";
				break;
			case FT_INT16:
				enum_name = "FT_INT16";
				break;
			case FT_INT24:
				enum_name = "FT_INT24";
				break;
			case FT_INT32:
				enum_name = "FT_INT32";
				break;
			case FT_DOUBLE:
				enum_name = "FT_DOUBLE";
				break;
			case FT_ABSOLUTE_TIME:
				enum_name = "FT_ABSOLUTE_TIME";
				break;
			case FT_RELATIVE_TIME:
				enum_name = "FT_RELATIVE_TIME";
				break;
			case FT_NSTRING_UINT8:
				enum_name = "FT_NSTRING_UINT8";
				break;
			case FT_STRING:
				enum_name = "FT_STRING";
				break;
			case FT_ETHER:
				enum_name = "FT_ETHER";
				break;
			case FT_BYTES:
				enum_name = "FT_BYTES";
				break;
			case FT_IPv4:
				enum_name = "FT_IPv4";
				break;
			case FT_IPv6:
				enum_name = "FT_IPv6";
				break;
			case FT_IPXNET:
				enum_name = "FT_IPXNET";
				break;
			case FT_TEXT_ONLY:
				enum_name = "FT_TEXT_ONLY";
				break;
			default:
				g_assert_not_reached();
				enum_name = NULL;
			}
			printf("F\t%s\t%s\t%s\t%s\n", hfinfo->name, hfinfo->abbrev,
				enum_name,parent_hfinfo->abbrev);
		}
	}
}
