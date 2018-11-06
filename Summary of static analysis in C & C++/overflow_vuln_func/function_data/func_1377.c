static gboolean
pppdump_read(wtap *wth, int *err, int *data_offset)
{
	gboolean	retval;
	int		num_bytes;
	direction_enum	direction;
	guint8		*buf;
	pppdump_t	*state;
	pkt_id		*pid;

	buffer_assure_space(wth->frame_buffer, 8192);
	buf = buffer_start_ptr(wth->frame_buffer);

	state = wth->capture.generic;
	pid = g_new(pkt_id, 1);
	if (!pid) {
		*err = errno;	/
		return FALSE;
	}
	pid->offset = 0;
	pid->num_saved_states = 0;

	retval = collate(state, wth->fh, err, buf, &num_bytes, &direction, pid);

	if (!retval) {
		g_free(pid);
		return FALSE;
	}

	pid->dir = direction;

	g_ptr_array_add(state->pids, pid);
	/
	*data_offset = state->pkt_cnt;
	state->pkt_cnt++;

	wth->phdr.len		= num_bytes;
	wth->phdr.caplen	= num_bytes;
	wth->phdr.ts.tv_sec	= state->timestamp;
	wth->phdr.ts.tv_usec	= state->tenths * 100000;
	wth->phdr.pkt_encap	= WTAP_ENCAP_PPP_WITH_PHDR;

	wth->pseudo_header.p2p.sent = (direction == DIRECTION_SENT ? TRUE : FALSE);

	return TRUE;
}
