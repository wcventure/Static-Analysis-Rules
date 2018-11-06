	
int
pppdump_open(wtap *wth, int *err)
{
	guint8		buffer[6];	/
	pppdump_t	*state;

	/

	file_seek(wth->fh, 0, SEEK_SET); 
	wtap_file_read_unknown_bytes(buffer, sizeof(buffer), wth->fh, err);

	if (buffer[0] == PPPD_RESET_TIME &&
			(buffer[5] == PPPD_SENT_DATA ||
			 buffer[5] == PPPD_RECV_DATA ||
			 buffer[5] == PPPD_TIME_STEP_LONG ||
			 buffer[5] == PPPD_TIME_STEP_SHORT ||
			 buffer[5] == PPPD_RESET_TIME)) {

		goto my_file_type;
	}
	else {
		return 0;
	}

  my_file_type:

	state = wth->capture.generic = g_malloc(sizeof(pppdump_t));
	state->timestamp = pntohl(&buffer[1]);
	state->tenths = 0;

	init_state(state);

	state->offset = 5; 
	file_seek(wth->fh, 5, SEEK_SET); 
	wth->file_encap = WTAP_ENCAP_PPP_WITH_PHDR; 
	wth->file_type = WTAP_FILE_PPPDUMP; 

	wth->snapshot_length = 8192; / 
	wth->subtype_read = pppdump_read; 
	wth->subtype_seek_read = pppdump_seek_read; 
	wth->subtype_close = pppdump_close;

	state->seek_state = g_malloc(sizeof(pppdump_t));

	state->pids = g_ptr_array_new();
	state->pkt_cnt = 0;
	state->num_saved_states = 0;

	return 1;
}
