static void
find_rsvp_session_tempfilt(tvbuff_t *tvb, int hdr_offset, int *session_offp, int *tempfilt_offp)
{
    int s_off = 0, t_off = 0;
    int len, off;
    guint16 obj_length;

    if (!tvb_bytes_exist(tvb, hdr_offset+6, 2))
	goto done;

    len = tvb_get_ntohs(tvb, hdr_offset+6) + hdr_offset;
    off = hdr_offset + 8;
    for (off = hdr_offset + 8; off < len && tvb_bytes_exist(tvb, off, 3);
    	 off += obj_length) {
	obj_length = tvb_get_ntohs(tvb, off);
	if (obj_length == 0)
	    break;
	switch(tvb_get_guint8(tvb, off+2)) {
	case RSVP_CLASS_SESSION:
	    s_off = off;
	    break;
	case RSVP_CLASS_SENDER_TEMPLATE:
	case RSVP_CLASS_FILTER_SPEC:
	    t_off = off;
	    break;
	default:
	    break;
	}
    }

 done:
    if (session_offp) *session_offp = s_off;
    if (tempfilt_offp) *tempfilt_offp = t_off;
}
