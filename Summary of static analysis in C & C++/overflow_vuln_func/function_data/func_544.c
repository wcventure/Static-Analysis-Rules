static const char *
bitrange0(v, s, buf, buflen)
	guint32 v;
	int s;
	char *buf;
	int buflen;
{
	guint32 v0;
	char *p, *ep;
	int off;
	int i, l;

	if (buflen < 1)
		return NULL;
	if (buflen == 1) {
		buf[0] = '\0';
		return NULL;
	}

	v0 = v;
	p = buf;
	ep = buf + buflen - 1;
	memset(buf, 0, buflen);
	off = 0;
	while (off < 32) {
		/
		if ((v & 0x01) == 0) {
			switch (v & 0x0f) {
			case 0x00:
				v >>= 4; off += 4; continue;
			case 0x08:
				v >>= 3; off += 3; continue;
			case 0x04: case 0x0c:
				v >>= 2; off += 2; continue;
			default:
				v >>= 1; off += 1; continue;
			}
		}

		/
		for (i = 0; i < 32 - off; i++) {
			if ((v & (0x01 << i)) == 0)
				break;
		}
		if (i == 1)
			l = snprintf(p, ep - p, ",%d", s + off);
		else {
			l = snprintf(p, ep - p, ",%d-%d", s + off,
			    s + off + i - 1);
		}
		if (l > ep - p) {
			buf[0] = '\0';
			return NULL;
		}
		v >>= i; off += i;
	}

	return buf;
}
