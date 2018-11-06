static char *
wv_csp13_opaque_binary_tag(tvbuff_t *tvb, guint32 offset,
			   guint8 token, guint8 codepage, guint32 *length)
{
	guint32 data_len = tvb_get_guintvar(tvb, offset, length);
	char *str = NULL;

	switch (codepage)
		{
		case 0: /
			switch (token)
				{
				case 0x0B: /
				case 0x0F: /
				case 0x1A: /
				case 0x3C: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				case 0x11: /
					str = wv_datetime_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 1: /
			switch (token)
				{
				case 0x1C: /
				case 0x25: /
				case 0x26: /
				case 0x27: /
				case 0x28: /
				case 0x32: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 3: /
			switch (token)
				{
				case 0x06: /
				case 0x0C: /
				case 0x0D: /
				case 0x0E: /
				case 0x12: /
				case 0x13: /
					/
				case 0x16: /
				case 0x17: /
				case 0x18: /
				case 0x19: /
				case 0x1B: /
				case 0x1C: /
				case 0x1F: /
				case 0x21: /
				case 0x24: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 5: /
			switch (token)
				{
					/
					/ /
				case 0x3C: /
				case 0x3D: /
				case 0x3E: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 6: /
			switch (token)
				{
				case 0x1A: /
					/
				case 0x1C: /
					str = wv_datetime_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 9: /
			switch (token)
				{
				case 0x08: /
				case 0x0A: /
					/
				case 0x25: /
				case 0x28: /
				case 0x30: /
				case 0x3A: /
				case 0x3B: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;

		case 10:
			switch (token)
				{
					/
				case 0x0C: /
					str = wv_integer_from_opaque(tvb, offset + *length, data_len);
					break;
				default:
					break;
				}
			break;
		default:
			break;
		}

	if (str == NULL)
		{ /
			str = wmem_strdup_printf(wmem_packet_scope(), "(%d bytes of unparsed opaque data)", data_len);
		}
	*length += data_len;

	return str;
}
