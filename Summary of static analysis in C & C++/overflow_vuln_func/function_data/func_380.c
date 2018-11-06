static int
parse_toshiba_rec_hdr(wtap *wth, FILE *fh, int *err)
{
	char	line[TOSHIBA_LINE_LENGTH];
	int	num_items_scanned;
	int	pkt_len, pktnum, hr, min, sec, csec;
	char	channel[10], direction[10];

	/
	if (file_gets(line, TOSHIBA_LINE_LENGTH, fh) < 0) {
		*err = file_error(fh);
		if (*err == 0) {
			*err = WTAP_ERR_SHORT_READ;
		}
		return -1;
	}

	/
	num_items_scanned = sscanf(line, "%d] %d:%d:%d.%d %s %s",
			&pktnum, &hr, &min, &sec, &csec, channel, direction);

	if (num_items_scanned != 7) {
		*err = WTAP_ERR_BAD_RECORD;
		return -1;
	}

	/
	if (file_gets(line, TOSHIBA_LINE_LENGTH, fh) < 0) {
		*err = file_error(fh);
		if (*err == 0) {
			*err = WTAP_ERR_SHORT_READ;
		}
		return -1;
	}

	num_items_scanned = sscanf(line+64, "LEN=%d", &pkt_len);
	if (num_items_scanned != 1) {
		*err = WTAP_ERR_BAD_RECORD;
		return -1;
	}

	if (wth) {
		wth->phdr.ts.tv_sec = 0;
		wth->phdr.ts.tv_usec = 0;
		wth->phdr.caplen = pkt_len;
		wth->phdr.len = pkt_len;
		switch (channel[0]) {
			case 'B':
				wth->phdr.pkt_encap = WTAP_ENCAP_PPP;
				break;

			case 'D':
				wth->phdr.pkt_encap = WTAP_ENCAP_LAPD;
				wth->phdr.pseudo_header.lapd.from_network_to_user = 
					(direction[0] == 'R' ? TRUE : FALSE );
				break;
			
			default:
				wth->phdr.pkt_encap = WTAP_ENCAP_ETHERNET;
				break;
		}
	}
	return pkt_len;
}
