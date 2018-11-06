static gboolean netmon_read(wtap *wth, int *err, gchar **err_info,
    gint64 *data_offset)
{
	netmon_t *netmon = (netmon_t *)wth->priv;
	guint32	packet_size = 0;
	guint32 orig_size = 0;
	int	bytes_read;
	union {
		struct netmonrec_1_x_hdr hdr_1_x;
		struct netmonrec_2_x_hdr hdr_2_x;
	}	hdr;
	union {
		struct netmonrec_2_1_trlr trlr_2_1;
		struct netmonrec_2_2_trlr trlr_2_2;
		struct netmonrec_2_3_trlr trlr_2_3;
	}	trlr;
	int	hdr_size = 0;
	int	trlr_size = 0;
	int	rec_offset;
	guint8	*data_ptr;
	gint64	delta = 0;	/
	time_t	secs;
	guint32	usecs;
	double	t;
	guint16 network;

again:
	/
	if (netmon->current_frame >= netmon->frame_table_size) {
		/
		g_free(netmon->frame_table);
		netmon->frame_table = NULL;
		*err = 0;	/
		return FALSE;
	}

	/
	rec_offset = netmon->frame_table[netmon->current_frame];
	if (wth->data_offset != rec_offset) {
		if (rec_offset < wth->data_offset) {
			*err = WTAP_ERR_BAD_RECORD;
			*err_info = g_strdup("netmon: Record offset is in the middle of an earlier record");
			return FALSE;
		}
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
		*err = WTAP_ERR_BAD_RECORD;
		*err_info = g_strdup_printf("netmon: File has %u-byte packet, bigger than maximum of %u",
		    packet_size, WTAP_MAX_PACKET_SIZE);
		return FALSE;
	}

	*data_offset = wth->data_offset;

	/
	switch (wth->file_encap) {

	case WTAP_ENCAP_ATM_PDUS:
		if (packet_size < sizeof (struct netmon_atm_hdr)) {
			/
			*err = WTAP_ERR_BAD_RECORD;
			*err_info = g_strdup_printf("netmon: ATM file has a %u-byte packet, too small to have even an ATM pseudo-header",
			    packet_size);
			return FALSE;
		}
		if (!netmon_read_atm_pseudoheader(wth->fh, &wth->pseudo_header,
		    err))
			return FALSE;	/

		/
		orig_size -= (guint)sizeof (struct netmon_atm_hdr);
		packet_size -= (guint)sizeof (struct netmon_atm_hdr);
		wth->data_offset += sizeof (struct netmon_atm_hdr);
		break;

	case WTAP_ENCAP_ETHERNET:
		/
		wth->pseudo_header.eth.fcs_len = 0;
		break;
	}

	buffer_assure_space(wth->frame_buffer, packet_size);
	data_ptr = buffer_start_ptr(wth->frame_buffer);
	if (!netmon_read_rec_data(wth->fh, data_ptr, packet_size, err))
		return FALSE;	/
	wth->data_offset += packet_size;

	t = (double)netmon->start_usecs;
	switch (netmon->version_major) {

	case 1:
		/
		delta = ((guint32)pletohl(&hdr.hdr_1_x.ts_delta))*1000;
		break;

	case 2:
		delta = pletohl(&hdr.hdr_2_x.ts_delta_lo)
		    | (((guint64)pletohl(&hdr.hdr_2_x.ts_delta_hi)) << 32);
		break;
	}
	t += (double)delta;
	secs = (time_t)(t/1000000);
	usecs = (guint32)(t - (double)secs*1000000);
	wth->phdr.ts.secs = netmon->start_secs + secs;
	wth->phdr.ts.nsecs = usecs * 1000;
	wth->phdr.caplen = packet_size;
	wth->phdr.len = orig_size;

	/
	if (wth->file_encap == WTAP_ENCAP_ATM_PDUS) {
		atm_guess_traffic_type(data_ptr, packet_size,
		    &wth->pseudo_header);
	}

	/
	if ((netmon->version_major == 2 && netmon->version_minor >= 1) ||
	    netmon->version_major > 2) {
	    	if (netmon->version_major > 2) {
	    		/
			trlr_size = sizeof (struct netmonrec_2_3_trlr);
	    	} else {
			switch (netmon->version_minor) {

			case 1:
				trlr_size = sizeof (struct netmonrec_2_1_trlr);
				break;

			case 2:
				trlr_size = sizeof (struct netmonrec_2_2_trlr);
				break;

			default:
				trlr_size = sizeof (struct netmonrec_2_3_trlr);
				break;
			}
		}
		errno = WTAP_ERR_CANT_READ;

		bytes_read = file_read(&trlr, 1, trlr_size, wth->fh);
		if (bytes_read != trlr_size) {
			*err = file_error(wth->fh);
			if (*err == 0 && bytes_read != 0) {
				*err = WTAP_ERR_SHORT_READ;
			}
			return FALSE;
		}
		wth->data_offset += trlr_size;

		network = pletohs(trlr.trlr_2_1.network);
		if ((network & 0xF000) == NETMON_NET_PCAP_BASE) {
			/
			network &= 0x0FFF;
			wth->phdr.pkt_encap =
			    wtap_pcap_encap_to_wtap_encap(network);
			if (wth->phdr.pkt_encap == WTAP_ENCAP_UNKNOWN) {
				*err = WTAP_ERR_UNSUPPORTED_ENCAP;
				*err_info = g_strdup_printf("netmon: converted pcap network type %u unknown or unsupported",
				    network);
				return FALSE;
			}
		} else if (network < NUM_NETMON_ENCAPS) {
			/
			wth->phdr.pkt_encap = netmon_encap[network];
			if (wth->phdr.pkt_encap == WTAP_ENCAP_UNKNOWN) {
				*err = WTAP_ERR_UNSUPPORTED_ENCAP;
				*err_info = g_strdup_printf("netmon: network type %u unknown or unsupported",
				    network);
				return FALSE;
			}
		} else {
			/
			switch (network) {

			case NETMON_NET_NETEVENT:
			case NETMON_NET_NETWORK_INFO_EX:
			case NETMON_NET_PAYLOAD_HEADER:
			case NETMON_NET_NETWORK_INFO:
			case NETMON_NET_DNS_CACHE:
			case NETMON_NET_NETMON_FILTER:
				/
				goto again;

			default:
				*err = WTAP_ERR_UNSUPPORTED_ENCAP;
				*err_info = g_strdup_printf("netmon: network type %u unknown or unsupported",
				    network);
				return FALSE;
			}
		}
	}

	return TRUE;
}
