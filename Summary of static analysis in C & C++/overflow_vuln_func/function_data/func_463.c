void
display_epoch_time(gchar *buf, int buflen, time_t sec, gint32 frac,
    time_res_t units)
{
	double elapsed_secs;

	elapsed_secs = difftime(sec,(time_t)0);

	/
	if (frac < 0) {
		frac = -frac;
		if (elapsed_secs >= 0)
			buf[0] = '-';
			++buf;
	}
	switch (units) {

	case SECS:
		g_snprintf(buf, buflen, "%0.0f", elapsed_secs);
		break;

	case DSECS:
		g_snprintf(buf, buflen, "%0.0f.%02d", elapsed_secs, frac);
		break;

	case CSECS:
		g_snprintf(buf, buflen, "%0.0f.%02d", elapsed_secs, frac);
		break;

	case MSECS:
		g_snprintf(buf, buflen, "%0.0f.%03d", elapsed_secs, frac);
		break;

	case USECS:
		g_snprintf(buf, buflen, "%0.0f.%06d", elapsed_secs, frac);
		break;

	case NSECS:
		g_snprintf(buf, buflen, "%0.0f.%09d", elapsed_secs, frac);
		break;
	}
}
