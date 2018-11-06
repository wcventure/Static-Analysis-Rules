static char *
emnc10_opaque_binary_attr(tvbuff_t *tvb, guint32 offset,
			  guint8 token, guint8 codepage, guint32 *length)
{
	guint32 data_len = tvb_get_guintvar(tvb, offset, length);
	char *str = NULL;

	switch (codepage) {
	case 0: /
		switch (token) {
		case 0x05: /
			str = date_time_from_opaque(tvb,
						    offset + *length, data_len);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	if (str == NULL) { /
		str = wmem_strdup_printf(wmem_packet_scope(), "(%d bytes of unparsed opaque data)", data_len);
	}
	*length += data_len;

	return str;
}
