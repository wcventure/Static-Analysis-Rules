static guint64 ns_hrtime2nsec(guint32 tm)
{
	guint32	val = tm & NSPR_HRTIME_MASKTM;
	switch(tm & NSPR_HRTIME_MASKFMT)
	{
	case NSPR_HRTIME_SEC:	return (guint64)val*1000000000;
	case NSPR_HRTIME_MSEC:	return (guint64)val*1000000;
	case NSPR_HRTIME_USEC:	return (guint32)val*1000;
	case NSPR_HRTIME_NSEC:	return val;
	}
	return tm;
}
