const char *wtap_strerror(int err)
{
	static char errbuf[6+11+1];	/
	int wtap_errlist_index;

	if (err < 0) {
#ifdef HAVE_LIBZ
		if (err >= WTAP_ERR_ZLIB_MIN && err <= WTAP_ERR_ZLIB_MAX) {
			/
			sprintf(errbuf, "Uncompression error: %s",
			    zError(err - WTAP_ERR_ZLIB));
			return errbuf;
		}
#endif
		wtap_errlist_index = -1 - err;
		if (wtap_errlist_index >= WTAP_ERRLIST_SIZE) {
			sprintf(errbuf, "Error %d", err);
			return errbuf;
		}
		if (wtap_errlist[wtap_errlist_index] == NULL)
			return "Unknown reason";
		return wtap_errlist[wtap_errlist_index];
	} else
		return strerror(err);
}
