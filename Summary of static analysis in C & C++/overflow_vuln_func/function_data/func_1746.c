void
display_epoch_time(gchar *buf, int buflen, time_t sec, gint32 frac,
    time_res_t units)
{
	const char *sign;
	double elapsed_secs;

	elapsed_secs = difftime(sec,(time_t)0); 

	/
	sign = "";
	if (frac < 0) {
		frac = -frac;
		if (elapsed_secs >= 0)
			sign = "-";
	}
	switch (units) {

	case SECS:
		g_snprintf(buf, buflen, "%s%0.0f", sign, elapsed_secs);
		break;

	case DSECS:
		g_snprintf(buf, buflen, "%s%0.0f.%01d", sign, elapsed_secs, frac);
		break;

	case CSECS:
		g_snprintf(buf, buflen, "%s%0.0f.%02d", sign, elapsed_secs, frac);
		break;

	case MSECS:
		g_snprintf(buf, buflen, "%s%0.0f.%03d", sign, elapsed_secs, frac);
		break;

	case USECS:
		g_snprintf(buf, buflen, "%s%0.0f.%06d", sign, elapsed_secs, frac);
		break;

	case NSECS:
		g_snprintf(buf, buflen, "%s%0.0f.%09d", sign, elapsed_secs, frac);
		break;
	}
}
