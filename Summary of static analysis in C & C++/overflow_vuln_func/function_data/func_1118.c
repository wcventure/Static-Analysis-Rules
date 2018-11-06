static gchar *
id_to_str(const guint8 *ad) {

	static gchar	*str[17];
	gchar		*p;
	guint8		bits8to5, bits4to1, i;
	static const	gchar hex_digits[10] = "0123456789";

	p = (gchar *)&str[17];
	*--p = '\0';
	i = 7;
	for (;;) {
		bits8to5 = (ad[i] >> 4) & 0x0F;
		bits4to1 = ad[i] & 0x0F;
		if (bits8to5 < 0xA) *--p = hex_digits[bits8to5];
		if (bits4to1 < 0xA) *--p = hex_digits[bits4to1];
		if (i == 0) break;
		i--;
	}
	return p;
}
