int ngsniffer_open(wtap *wth, int *err)
{
	int bytes_read;
	char magic[sizeof ngsniffer_magic];
	char record_type[2];
	char record_length[4]; /
	guint16 type, length;
	struct vers_rec version;
	guint16	start_date;
	guint16	start_time;
	static const int sniffer_encap[] = {
		WTAP_ENCAP_TOKEN_RING,
		WTAP_ENCAP_ETHERNET,
		WTAP_ENCAP_ARCNET,
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_UNKNOWN,	/
		WTAP_ENCAP_FDDI_BITSWAPPED,
		WTAP_ENCAP_ATM_SNIFFER
	};
	#define NUM_NGSNIFF_ENCAPS (sizeof sniffer_encap / sizeof sniffer_encap[0])
	struct tm tm;

	/
	file_seek(wth->fh, 0, SEEK_SET);
	wth->data_offset = 0;
	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(magic, 1, sizeof magic, wth->fh);
	if (bytes_read != sizeof magic) {
		*err = file_error(wth->fh);
		if (*err != 0)
			return -1;
		return 0;
	}
	wth->data_offset += sizeof magic;

	if (memcmp(magic, ngsniffer_magic, sizeof ngsniffer_magic)) {
		return 0;
	}

	/
	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(record_type, 1, 2, wth->fh);
	bytes_read += file_read(record_length, 1, 4, wth->fh);
	if (bytes_read != 6) {
		*err = file_error(wth->fh);
		if (*err != 0)
			return -1;
		return 0;
	}
	wth->data_offset += 6;

	type = pletohs(record_type);
	length = pletohs(record_length);

	if (type != REC_VERS) {
		g_message("ngsniffer: Sniffer file doesn't start with a version record");
		*err = WTAP_ERR_BAD_RECORD;
		return -1;
	}

	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(&version, 1, sizeof version, wth->fh);
	if (bytes_read != sizeof version) {
		*err = file_error(wth->fh);
		if (*err != 0)
			return -1;
		return 0;
	}
	wth->data_offset += sizeof version;

	/
	if (version.network >= NUM_NGSNIFF_ENCAPS
	    || (sniffer_encap[version.network] == WTAP_ENCAP_UNKNOWN
	       && version.network != 7)) {
		g_message("ngsniffer: network type %u unknown or unsupported",
		    version.network);
		*err = WTAP_ERR_UNSUPPORTED_ENCAP;
		return -1;
	}

	/
	if (version.timeunit >= NUM_NGSNIFF_TIMEUNITS) {
		g_message("ngsniffer: Unknown timeunit %u", version.timeunit);
		*err = WTAP_ERR_UNSUPPORTED;
		return -1;
	}

	/
	if (version.format != 1) {
		wth->file_type = WTAP_FILE_NGSNIFFER_COMPRESSED;

	} else {
		wth->file_type = WTAP_FILE_NGSNIFFER_UNCOMPRESSED;
	}

	/
	wth->file_encap = sniffer_encap[version.network];

	/
	if (skip_header_records(wth, err, version.maj_vers) < 0)
		return -1;

	/
	if (wth->random_fh != NULL)
		file_seek(wth->random_fh, wth->data_offset, SEEK_SET);

	/
	wth->capture.ngsniffer = g_malloc(sizeof(ngsniffer_t));

	/
	wth->capture.ngsniffer->seq.buf = NULL;
	wth->capture.ngsniffer->rand.buf = NULL;

	/
	wth->capture.ngsniffer->seq.uncomp_offset = wth->data_offset;
	wth->capture.ngsniffer->seq.comp_offset = wth->data_offset;
	wth->capture.ngsniffer->rand.uncomp_offset = wth->data_offset;
	wth->capture.ngsniffer->rand.comp_offset = wth->data_offset;

	/
	wth->capture.ngsniffer->first_blob = NULL;
	wth->capture.ngsniffer->last_blob = NULL;
	wth->capture.ngsniffer->current_blob = NULL;

	wth->subtype_read = ngsniffer_read;
	wth->subtype_seek_read = ngsniffer_seek_read;
	wth->subtype_sequential_close = ngsniffer_sequential_close;
	wth->subtype_close = ngsniffer_close;
	wth->snapshot_length = 16384;	/
	wth->capture.ngsniffer->timeunit = Usec[version.timeunit];
	wth->capture.ngsniffer->is_atm =
	    (wth->file_encap == WTAP_ENCAP_ATM_SNIFFER);

	/
	start_time = pletohs(&version.time);
	start_date = pletohs(&version.date);
	tm.tm_year = ((start_date&0xfe00)>>9) + 1980 - 1900;
	tm.tm_mon = ((start_date&0x1e0)>>5) - 1;
	tm.tm_mday = (start_date&0x1f);
	/
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_isdst = -1;
	wth->capture.ngsniffer->start = mktime(&tm);
	/

	return 1;
}
