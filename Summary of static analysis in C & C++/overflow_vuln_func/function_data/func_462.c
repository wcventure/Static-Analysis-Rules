void
display_signed_time(gchar *buf, int buflen, gint32 sec, gint32 frac,
    time_res_t units)
{
	/
	if (frac < 0) {
		frac = -frac;
		if (sec >= 0)
			buf[0] = '-';
			++buf;	}
	switch (units) {

	case SECS:
		g_snprintf(buf, buflen, "%d", sec);
		break;

	case DSECS:
		g_snprintf(buf, buflen, "%d.%01d", sec, frac);
		break;

	case CSECS:
		g_snprintf(buf, buflen, "%d.%02d", sec, frac);
		break;

	case MSECS:
		g_snprintf(buf, buflen, "%d.%03d", sec, frac);
		break;

	case USECS:
		g_snprintf(buf, buflen, "%d.%06d", sec, frac);
		break;

	case NSECS:
		g_snprintf(buf, buflen, "%d.%09d", sec, frac);
		break;
	}
}
