static gchar *
imsi_to_str(const guint8 *ad) {

	static gchar	*str[16];
	gchar		*p;
	guint8		i, j = 0;
	
	p = (gchar *)&str[0];
	for (i=0;i<8;i++) {
		if ((ad[i] & 0x0F) <= 9) p[j++] = (ad[i] & 0x0F) + 0x30;
		if (((ad[i] >> 4) & 0x0F) <= 9) p[j++] = ((ad[i] >> 4) & 0x0F) + 0x30;
	}
	p[j] = 0;
	
	return p;
}
