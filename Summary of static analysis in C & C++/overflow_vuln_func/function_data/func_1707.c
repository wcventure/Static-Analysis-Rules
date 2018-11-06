static gboolean
snoop_read_shomiti_wireless_pseudoheader(FILE_T fh,
    union wtap_pseudo_header *pseudo_header, int *err, int *header_size)
{
	shomiti_wireless_header whdr;
	int	bytes_read;
	char buffer[250];
	int rsize;

	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(&whdr, 1, sizeof (shomiti_wireless_header), fh);
	if (bytes_read != sizeof (shomiti_wireless_header)) {
		*err = file_error(fh);
		if (*err == 0)
			*err = WTAP_ERR_SHORT_READ;
		return FALSE;
	}

	/
	rsize = ((int) whdr.pad[3]) - 8;
	if(rsize > 0) {
	    bytes_read = file_read(buffer, 1, rsize, fh);
	    if (bytes_read != rsize) {
		*err = file_error(fh);
		if (*err == 0)
		    *err = WTAP_ERR_SHORT_READ;
		return FALSE;
	    }
	}

	pseudo_header->ieee_802_11.fcs_len = 4;
	pseudo_header->ieee_802_11.channel = whdr.channel;
	pseudo_header->ieee_802_11.data_rate = whdr.rate;
	pseudo_header->ieee_802_11.signal_level = whdr.signal;

	/
	if(header_size != NULL)
	    *header_size = rsize + 8 + 4;

    return TRUE;
}
