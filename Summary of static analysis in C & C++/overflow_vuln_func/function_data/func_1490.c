static gchar *
msisdn_to_str(const guint8 *ad, int len) {

	static gchar	str[18] = "+                ";
	guint8		bits8to5, bits4to1;
	int		i, j = 1;
	static const	gchar hex_digits[10] = "0123456789";

	for (i = 1; i < len && i < 9; i++) {
		bits8to5 = (ad[i] >> 4) & 0x0F;
		bits4to1 = ad[i] & 0x0F;
		if (bits4to1 < 0xA) 
			str[j++] = hex_digits[bits4to1];
		if (bits8to5 < 0xA) 
			str[j++] = hex_digits[bits8to5];
	}
	str[j] = '\0';
	
	return str;
}
