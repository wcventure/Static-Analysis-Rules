int netxray_open(wtap *wth, int *err, gchar **err_info)
{
	int bytes_read;
	char magic[sizeof netxray_magic];
	gboolean is_old;
	struct netxray_hdr hdr;
	guint network_type;
	double timeunit;
	int version_major, version_minor;
	int file_type;
	double start_timestamp;
	static const int netxray_encap[] = {
		WTAP_ENCAP_UNKNOWN,
		WTAP_ENCAP_ETHERNET,
		WTAP_ENCAP_TOKEN_RING,
		WTAP_ENCAP_FDDI_BITSWAPPED,
		/
		WTAP_ENCAP_ETHERNET,		/
		WTAP_ENCAP_UNKNOWN,		/
		WTAP_ENCAP_UNKNOWN,		/
		WTAP_ENCAP_UNKNOWN,		/
		WTAP_ENCAP_UNKNOWN,		/
		WTAP_ENCAP_ATM_PDUS_UNTRUNCATED,/
		WTAP_ENCAP_IEEE_802_11_WITH_RADIO,
						/
		WTAP_ENCAP_UNKNOWN		/
	};
	#define NUM_NETXRAY_ENCAPS (sizeof netxray_encap / sizeof netxray_encap[0])
	int file_encap;
	guint isdn_type = 0;

	/
	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(magic, 1, sizeof magic, wth->fh);
	if (bytes_read != sizeof magic) {
		*err = file_error(wth->fh);
		if (*err != 0)
			return -1;
		return 0;
	}
	wth->data_offset += sizeof magic;

	if (memcmp(magic, netxray_magic, sizeof magic) == 0) {
		is_old = FALSE;
	} else if (memcmp(magic, old_netxray_magic, sizeof magic) == 0) {
		is_old = TRUE;
	} else {
		return 0;
	}

	/
	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(&hdr, 1, sizeof hdr, wth->fh);
	if (bytes_read != sizeof hdr) {
		*err = file_error(wth->fh);
		if (*err != 0)
			return -1;
		return 0;
	}
	wth->data_offset += sizeof hdr;

	if (is_old) {
		version_major = 0;
		version_minor = 0;
		file_type = WTAP_FILE_NETXRAY_OLD;
	} else {
		/
		if (memcmp(hdr.version, vers_1_0, sizeof vers_1_0) == 0) {
			version_major = 1;
			version_minor = 0;
			file_type = WTAP_FILE_NETXRAY_1_0;
		} else if (memcmp(hdr.version, vers_1_1, sizeof vers_1_1) == 0) {
			version_major = 1;
			version_minor = 1;
			file_type = WTAP_FILE_NETXRAY_1_1;
		} else if (memcmp(hdr.version, vers_2_000, sizeof vers_2_000) == 0) {
			version_major = 2;
			version_minor = 0;
			file_type = WTAP_FILE_NETXRAY_2_00x;
		} else if (memcmp(hdr.version, vers_2_001, sizeof vers_2_001) == 0) {
			version_major = 2;
			version_minor = 1;
			file_type = WTAP_FILE_NETXRAY_2_00x;
		} else if (memcmp(hdr.version, vers_2_002, sizeof vers_2_002) == 0) {
			version_major = 2;
			version_minor = 2;
			file_type = WTAP_FILE_NETXRAY_2_00x;
		} else if (memcmp(hdr.version, vers_2_003, sizeof vers_2_003) == 0) {
			version_major = 2;
			version_minor = 3;
			file_type = WTAP_FILE_NETXRAY_2_00x;
		} else {
			*err = WTAP_ERR_UNSUPPORTED;
			*err_info = g_strdup_printf("netxray: version \"%.8s\" unsupported", hdr.version);
			return -1;
		}
	}

	switch (hdr.network_plus) {

	case 0:
		/
		network_type = hdr.network + 1;
		break;

	case 2:
		/
		network_type = hdr.network;
		break;

	default:
		*err = WTAP_ERR_UNSUPPORTED;
		*err_info = g_strdup_printf("netxray: the byte after the network type has the value %u, which I don't understand",
		    hdr.network_plus);
		return -1;
	}

	if (network_type >= NUM_NETXRAY_ENCAPS
	    || netxray_encap[network_type] == WTAP_ENCAP_UNKNOWN) {
		*err = WTAP_ERR_UNSUPPORTED_ENCAP;
		*err_info = g_strdup_printf("netxray: network type %u (%u) unknown or unsupported",
		    network_type, hdr.network_plus);
		return -1;
	}

	/
	start_timestamp = (double)pletohl(&hdr.timelo)
	    + (double)pletohl(&hdr.timehi)*4294967296.0;
	switch (file_type) {

	case WTAP_FILE_NETXRAY_OLD:
		timeunit = 1000.0;
		wth->tsprecision = WTAP_FILE_TSPREC_MSEC;
		break;

	case WTAP_FILE_NETXRAY_1_0:
		timeunit = 1000.0;
		wth->tsprecision = WTAP_FILE_TSPREC_MSEC;
		break;

	case WTAP_FILE_NETXRAY_1_1:
		/
		timeunit = 1000000.0;
		wth->tsprecision = WTAP_FILE_TSPREC_USEC;
		break;

	case WTAP_FILE_NETXRAY_2_00x:
		/
		switch (network_type) {

		case 1:
			/
			switch (hdr.captype) {

			case CAPTYPE_NDIS:
				if (hdr.timeunit >= NUM_NETXRAY_TIMEUNITS) {
					*err = WTAP_ERR_UNSUPPORTED;
					*err_info = g_strdup_printf(
					    "netxray: Unknown timeunit %u for Ethernet/CAPTYPE_NDIS version %.8s capture",
					    hdr.timeunit, hdr.version);
					return -1;
				}
				timeunit = TpS[hdr.timeunit];
				break;

			case ETH_CAPTYPE_GIGPOD:
				if (hdr.timeunit > NUM_NETXRAY_TIMEUNITS_GIGPOD
				    || TpS_gigpod[hdr.timeunit] == 0.0) {
					*err = WTAP_ERR_UNSUPPORTED;
					*err_info = g_strdup_printf(
					    "netxray: Unknown timeunit %u for Ethernet/ETH_CAPTYPE_GIGPOD version %.8s capture",
					    hdr.timeunit, hdr.version);
					return -1;
				}
				timeunit = TpS_gigpod[hdr.timeunit];

				/
				if (version_minor == 2 || version_minor == 3)
					start_timestamp = 0.0;
				break;

			case ETH_CAPTYPE_OTHERPOD:
				if (hdr.timeunit > NUM_NETXRAY_TIMEUNITS_OTHERPOD
				    || TpS_otherpod[hdr.timeunit] == 0.0) {
					*err = WTAP_ERR_UNSUPPORTED;
					*err_info = g_strdup_printf(
					    "netxray: Unknown timeunit %u for Ethernet/ETH_CAPTYPE_OTHERPOD version %.8s capture",
					    hdr.timeunit, hdr.version);
					return -1;
				}
				timeunit = TpS_otherpod[hdr.timeunit];

				/
				if (version_minor == 2 || version_minor == 3)
					start_timestamp = 0.0;
				break;

			case ETH_CAPTYPE_OTHERPOD2:
				if (hdr.timeunit > NUM_NETXRAY_TIMEUNITS_OTHERPOD2
				    || TpS_otherpod2[hdr.timeunit] == 0.0) {
					*err = WTAP_ERR_UNSUPPORTED;
					*err_info = g_strdup_printf(
					    "netxray: Unknown timeunit %u for Ethernet/ETH_CAPTYPE_OTHERPOD2 version %.8s capture",
					    hdr.timeunit, hdr.version);
					return -1;
				}
				timeunit = TpS_otherpod2[hdr.timeunit];
				/
				if (version_minor == 2 || version_minor == 3)
					start_timestamp = 0.0;
				break;

			case ETH_CAPTYPE_GIGPOD2:
				if (hdr.timeunit > NUM_NETXRAY_TIMEUNITS_GIGPOD2
				    || TpS_gigpod2[hdr.timeunit] == 0.0) {
					*err = WTAP_ERR_UNSUPPORTED;
					*err_info = g_strdup_printf(
					    "netxray: Unknown timeunit %u for Ethernet/ETH_CAPTYPE_GIGPOD2 version %.8s capture",
					    hdr.timeunit, hdr.version);
					return -1;
				}
				timeunit = TpS_gigpod2[hdr.timeunit];
				break;

			default:
				*err = WTAP_ERR_UNSUPPORTED;
				*err_info = g_strdup_printf(
				    "netxray: Unknown capture type %u for Ethernet version %.8s capture",
				    hdr.captype, hdr.version);
				return -1;
			}
			break;

		default:
			if (hdr.timeunit > NUM_NETXRAY_TIMEUNITS) {
				*err = WTAP_ERR_UNSUPPORTED;
				*err_info = g_strdup_printf(
				    "netxray: Unknown timeunit %u for %u/%u version %.8s capture",
				    hdr.timeunit, network_type, hdr.captype,
				    hdr.version);
				return -1;
			}
			timeunit = TpS[hdr.timeunit];
			break;
		}

		/
		if (timeunit >= 1e7)
			wth->tsprecision = WTAP_FILE_TSPREC_NSEC;
		else
			wth->tsprecision = WTAP_FILE_TSPREC_USEC;
		break;

	default:
		g_assert_not_reached();
		timeunit = 0.0;
	}
	start_timestamp = start_timestamp/timeunit;

	if (network_type == 4) {
		/
		if (version_major == 2) {
			switch (hdr.captype) {

			case WAN_CAPTYPE_PPP:
				/
				file_encap = WTAP_ENCAP_PPP_WITH_PHDR;
				break;

			case WAN_CAPTYPE_FRELAY:
				/
				file_encap = WTAP_ENCAP_FRELAY_WITH_PHDR;
				break;

			case WAN_CAPTYPE_HDLC:
			case WAN_CAPTYPE_HDLC2:
				/
				switch (hdr.wan_hdlc_subsub_captype) {

				case 0:	/
					file_encap = WTAP_ENCAP_LAPB;
					break;

				case 1:	/
				case 2:	/
				case 3:	/
					file_encap = WTAP_ENCAP_ISDN;
					isdn_type = hdr.wan_hdlc_subsub_captype;
					break;

				default:
					*err = WTAP_ERR_UNSUPPORTED_ENCAP;
					*err_info = g_strdup_printf("netxray: WAN HDLC capture subsubtype 0x%02x unknown or unsupported",
					   hdr.wan_hdlc_subsub_captype);
					return -1;
				}
				break;

			case WAN_CAPTYPE_SDLC:
				/
				file_encap = WTAP_ENCAP_SDLC;
				break;

			default:
				*err = WTAP_ERR_UNSUPPORTED_ENCAP;
				*err_info = g_strdup_printf("netxray: WAN capture subtype 0x%02x unknown or unsupported",
				   hdr.captype);
				return -1;
			}
		} else
			file_encap = WTAP_ENCAP_ETHERNET;
	} else
		file_encap = netxray_encap[network_type];

	/
	wth->file_type = file_type;
	wth->capture.netxray = g_malloc(sizeof(netxray_t));
	wth->subtype_read = netxray_read;
	wth->subtype_seek_read = netxray_seek_read;
	wth->subtype_close = netxray_close;
	wth->file_encap = file_encap;
	wth->snapshot_length = 0;	/
	wth->capture.netxray->start_time = pletohl(&hdr.start_time);
	wth->capture.netxray->timeunit = timeunit;
	wth->capture.netxray->start_timestamp = start_timestamp;
	wth->capture.netxray->version_major = version_major;

	/
	wth->capture.netxray->fcs_valid = FALSE;
	switch (file_encap) {

	case WTAP_ENCAP_ETHERNET:
	case WTAP_ENCAP_IEEE_802_11_WITH_RADIO:
	case WTAP_ENCAP_ISDN:
		/
		if (version_major == 2) {
			if (hdr.realtick[1] == 0x34 && hdr.realtick[2] == 0x12)
				wth->capture.netxray->fcs_valid = TRUE;
		}
		break;
	}

	/
	wth->capture.netxray->isdn_type = isdn_type;

	/
	wth->capture.netxray->wrapped      = FALSE;
	wth->capture.netxray->nframes      = pletohl(&hdr.nframes);
	wth->capture.netxray->start_offset = pletohl(&hdr.start_offset);
	wth->capture.netxray->end_offset   = pletohl(&hdr.end_offset);

	/
	if (file_seek(wth->fh, pletohl(&hdr.start_offset), SEEK_SET, err) == -1) {
		g_free(wth->capture.netxray);
		return -1;
	}
	wth->data_offset = pletohl(&hdr.start_offset);

	return 1;
}
