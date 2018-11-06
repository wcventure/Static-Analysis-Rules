static char *
sic10_opaque_literal_attr(tvbuff_t *tvb, guint32 offset,
			  const char *token, guint8 codepage _U_, guint32 *length)
{
	guint32 data_len = tvb_get_guintvar(tvb, offset, length);
	char *str = NULL;

	if ( token && ( (strcmp(token, "created") == 0)
			|| (strcmp(token, "si-expires") == 0) ) )
		{
			str = date_time_from_opaque(tvb, offset + *length, data_len);
		}
	if (str == NULL) { /
		str = wmem_strdup_printf(wmem_packet_scope(), "(%d bytes of unparsed opaque data)", data_len);
	}
	*length += data_len;

	return str;
}
