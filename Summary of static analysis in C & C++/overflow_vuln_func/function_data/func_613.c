static gboolean netmon_read(wtap *wth, int *err, long *data_offset)
{
	netmon_t *netmon = wth->capture.netmon;
	guint32	packet_size = 0;
	guint32 orig_size = 0;
	int	bytes_read;
	union {
		struct netmonrec_1_x_hdr hdr_1_x;
		struct netmonrec_2_x_hdr hdr_2_x;
	}	hdr;
	int	hdr_size = 0;
	int	rec_offset;
	guint8	*data_ptr;
	time_t	secs;
	guint32	usecs;
	double	t;

	/
	if (netmon->current_frame >= netmon->frame_table_size) {
		/
		g_free(wth->capture.netmon->frame_table);
		wth->capture.netmon->frame_table = NULL;
		*err = 0;	/
		return FALSE;
	}

	/
	rec_offset = netmon->frame_table[netmon->current_frame];
	if (wth->data_offset != rec_offset) {
		wth->data_offset = rec_offset;
		if (file_seek(wth->fh, wth->data_offset, SEEK_SET, err) == -1)
			return FALSE;
	}
	netmon->current_frame++;

	/
	switch (netmon->version_major) {

	case 1:
		hdr_size = sizeof (struct netmonrec_1_x_hdr);
		break;

	case 2:
		hdr_size = sizeof (struct netmonrec_2_x_hdr);
		break;
	}
	errno = WTAP_ERR_CANT_READ;

	bytes_read = file_read(&hdr, 1, hdr_size, wth->fh);
	if (bytes_read != hdr_size) {
		*err = file_error(wth->fh);
		if (*err == 0 && bytes_read != 0) {
			*err = WTAP_ERR_SHORT_READ;
		}
		return FALSE;
	}
	wth->data_offset += hdr_size;

	switch (netmon->version_major) {

	case 1:
		orig_size = pletohs(&hdr.hdr_1_x.orig_len);
		packet_size = pletohs(&hdr.hdr_1_x.incl_len);
		break;

	case 2:
		orig_size = pletohl(&hdr.hdr_2_x.orig_len);
		packet_size = pletohl(&hdr.hdr_2_x.incl_len);
		break;
	}
	if (packet_size > WTAP_MAX_PACKET_SIZE) {
		/
		g_message("netmon: File has %u-byte packet, bigger than maximum of %u",
		    packet_size, WTAP_MAX_PACKET_SIZE);
		*err = WTAP_ERR_BAD_RECORD;
		return FALSE;
	}

	*data_offset = wth->data_offset;

	/
	if (wth->file_encap == WTAP_ENCAP_ATM_PDUS) {
		if (packet_size < sizeof (struct netmon_atm_hdr)) {
			/
			g_message("netmon: ATM file has a %u-byte packet, too small to have even an ATM pseudo-header\n",
			    packet_size);
			*err = WTAP_ERR_BAD_RECORD;
			return FALSE;
		}
		if (!netmon_read_atm_pseudoheader(wth->fh, &wth->pseudo_header,
		    err))
			return FALSE;	/

		/
		orig_size -= sizeof (struct netmon_atm_hdr);
		packet_size -= sizeof (struct netmon_atm_hdr);
		wth->data_offset += sizeof (struct netmon_atm_hdr);
	}

	buffer_assure_space(wth->frame_buffer, packet_size);
	data_ptr = buffer_start_ptr(wth->frame_buffer);
	if (!netmon_read_rec_data(wth->fh, data_ptr, packet_size, err))
		return FALSE;	/
	wth->data_offset += packet_size;

	t = (double)netmon->start_usecs;
	switch (netmon->version_major) {

	case 1:
		t += ((double)pletohl(&hdr.hdr_1_x.ts_delta))*1000;
		break;

	case 2:
		t += (double)pletohl(&hdr.hdr_2_x.ts_delta_lo)
		    + (double)pletohl(&hdr.hdr_2_x.ts_delta_hi)*4294967296.0;
		break;
	}
	secs = (time_t)(t/1000000);
	usecs = (guint32)(t - secs*1000000);
	wth->phdr.ts.tv_sec = netmon->start_secs + secs;
	wth->phdr.ts.tv_usec = usecs;
	wth->phdr.caplen = packet_size;
	wth->phdr.len = orig_size;
	wth->phdr.pkt_encap = wth->file_encap;

	/
	if (wth->file_encap == WTAP_ENCAP_ATM_PDUS) {
		atm_guess_traffic_type(data_ptr, packet_size,
		    &wth->pseudo_header);
	}

	return TRUE;
}
