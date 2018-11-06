static gchar*
pduval_to_str(int type, int len, tvbuff_t *tvb, int offset)
{
	gchar *run;
	const guint8 *mac;
	int z, val;
	gchar *strval;

	run = textbuffer;
	run += sprintf(run, "Value: ");

	switch (type)
	{
		case IAPP_PDU_SSID:
			iaconvertbufftostr(run, tvb, offset + 3, len);
			break;
		case IAPP_PDU_BSSID:
		case IAPP_PDU_OLDBSSID:
		case IAPP_PDU_MSADDR:
			mac = tvb_get_ptr(tvb, offset + 3, len);
			for (z = 0; z < len; z++)
				run += sprintf(run, "%s%02x", z ? ":" : "", mac[z]);
			break;
		case IAPP_PDU_CAPABILITY:
		{
			int mask, first = 1;

			val = tvb_get_guint8(tvb, offset + 3);
			run += sprintf(run, "%02x (", val);
			for (mask = 0x80; mask; mask >>= 1)
				if (val & mask)
				{
					strval = match_strval(mask, iapp_cap_vals);
					if (strval)
					{
						if (!first)
							run += sprintf(run, " ");
						run += sprintf(run, strval);
					}
				}
			run += sprintf(run, ")");
			break;
		}
		case IAPP_PDU_ANNOUNCEINT:
			val = tvb_get_ntohs(tvb, offset + 3);
			run += sprintf(run, "%d seconds", val);
			break;
		case IAPP_PDU_HOTIMEOUT:
		case IAPP_PDU_BEACONINT:
			val = tvb_get_ntohs(tvb, offset + 3);
			run += sprintf(run, "%d Kus", val);
			break;
		case IAPP_PDU_MESSAGEID:
			val = tvb_get_ntohs(tvb, offset + 3);
			run += sprintf(run, "%d", val);
			break;
		case IAPP_PDU_PHYTYPE:
			val = tvb_get_guint8(tvb, offset + 3);
			strval = val_to_str(val, iapp_phy_vals, "Unknown");
			run += sprintf(run, strval);
                        is_fhss = (val == IAPP_PHY_FHSS);
			break;
		case IAPP_PDU_REGDOMAIN:
			val = tvb_get_guint8(tvb, offset + 3);
			strval = val_to_str(val, iapp_dom_vals, "Unknown");
			run += sprintf(run, strval);
			break;
		case IAPP_PDU_CHANNEL:
			val = tvb_get_guint8(tvb, offset + 3);
			if (is_fhss)
				run += sprintf(run, "Pattern set %d, sequence %d",
						((val >> 6) & 3) + 1, (val & 31) + 1);
			else
				run += sprintf(run, "%d", val);
			break;
		case IAPP_PDU_OUIIDENT:
			for (val = z = 0; z < 3; z++)
				val = (val << 8) | tvb_get_guint8(tvb, offset + 3 + z);
			strval = val_to_str(val, oui_vals, "Unknown");
			run += sprintf(run, strval);
			break;
	}

	return textbuffer;
}
