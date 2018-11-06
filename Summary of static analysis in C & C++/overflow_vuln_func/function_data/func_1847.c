static int
skip_header_records(wtap *wth, int *err, gint16 version)
{
	int bytes_read;
	char record_type[2];
	char record_length[4]; /
	guint16 type, length;
	int bytes_to_read;
	unsigned char buffer[32];

	for (;;) {
		errno = WTAP_ERR_CANT_READ;
		bytes_read = file_read(record_type, 1, 2, wth->fh);
		if (bytes_read != 2) {
			*err = file_error(wth->fh);
			if (*err != 0)
				return -1;
			if (bytes_read != 0) {
				*err = WTAP_ERR_SHORT_READ;
				return -1;
			}
			return 0;	/
		}

		type = pletohs(record_type);
		if ((type != REC_HEADER1) && (type != REC_HEADER2)
			&& ((type != REC_V2DESC) || (version > 2)) ) {
			/
			file_seek(wth->fh, -2, SEEK_CUR);
			return 0;
		}

		errno = WTAP_ERR_CANT_READ;
		bytes_read = file_read(record_length, 1, 4, wth->fh);
		if (bytes_read != 4) {
			*err = file_error(wth->fh);
			if (*err == 0)
				*err = WTAP_ERR_SHORT_READ;
			return -1;
		}
		wth->data_offset += 6;

		length = pletohs(record_length);

		/
		if (type == REC_HEADER2 &&
		    wth->file_encap == WTAP_ENCAP_UNKNOWN) {
			/
			bytes_to_read = length;
			if (length > sizeof buffer)
				length = sizeof buffer;
			bytes_read = file_read(buffer, 1, bytes_to_read,
			    wth->fh);
			if (bytes_read != bytes_to_read) {
				*err = file_error(wth->fh);
				if (*err == 0) {
					*err = WTAP_ERR_SHORT_READ;
					return -1;
				}
			}

			/
			switch (buffer[4]) {

			case NET_FRAME_RELAY:
				wth->file_encap = WTAP_ENCAP_FRELAY;
				break;

			case NET_PPP:
				wth->file_encap = WTAP_ENCAP_PPP;
				break;
			}

			/
			if (length > sizeof buffer) {
				file_seek(wth->fh, length - sizeof buffer,
				    SEEK_CUR);
			}
		} else {
			/
			file_seek(wth->fh, length, SEEK_CUR);
		}
		wth->data_offset += length;
	}
}
