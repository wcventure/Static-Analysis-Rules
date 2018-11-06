static gchar*
authval_to_str(int type, int len, tvbuff_t *tvb, int offset)
{
	gchar *run;
	int z, val;

	run = textbuffer;
	run += sprintf(run, "Value: ");

	switch (type)
	{
		case IAPP_AUTH_STATUS:
			strcpy(textbuffer, tvb_get_guint8(tvb, offset + 3) ? "Authenticated" : "Not authenticated");
			break;
		case IAPP_AUTH_USERNAME:
		case IAPP_AUTH_PROVNAME:
			iaconvertbufftostr(run, tvb, offset + 3, len);
			break;
		case IAPP_AUTH_RXPKTS:
		case IAPP_AUTH_TXPKTS:
		case IAPP_AUTH_RXBYTES:
		case IAPP_AUTH_TXBYTES:
		case IAPP_AUTH_RXGWORDS:
		case IAPP_AUTH_TXGWORDS:
		case IAPP_AUTH_VOLLIMIT:
			val = tvb_get_ntohl(tvb, offset + 3);
			run += sprintf(run, "%d", val);
			break;
		case IAPP_AUTH_LOGINTIME:
		case IAPP_AUTH_TIMELIMIT:
		case IAPP_AUTH_ACCCYCLE:
			val = tvb_get_ntohl(tvb, offset + 3);
                        run += sprintf(run, "%d seconds", val);
			break;
		case IAPP_AUTH_IPADDR:
			run += sprintf(run, "%d.%d.%d.%d",
				tvb_get_guint8(tvb, offset + 3),
				tvb_get_guint8(tvb, offset + 4),
				tvb_get_guint8(tvb, offset + 5),
				tvb_get_guint8(tvb, offset + 6));
			break;
		case IAPP_AUTH_TRAILER:
			for (z = 0; z < len; z++)
			run += sprintf(run, " %02x", tvb_get_guint8(tvb, offset + 3 + z));
			break;
	}

	return textbuffer;
}
