static gchar *
msisdn_to_str(const guint8 *ad, int len) {

	static gchar	*str[17];
	gchar		*p;
	guint8		bits8to5, bits4to1, i;
	static const	gchar hex_digits[16] = "0123456789      ";

	p = (gchar *)&str[0];
	*p = '+';
	i = 1;
	for (;;) {
		bits8to5 = (ad[i] >> 4) & 0x0F;
		bits4to1 = ad[i] & 0x0F;
		if (bits4to1 < 0xA) *++p = hex_digits[bits4to1];
		if (bits8to5 < 0xA) *++p = hex_digits[bits8to5];
		if (i == len-1) break;
		i++;
	}
	*++p = '\0';
	return (gchar *)&str[0];
}
