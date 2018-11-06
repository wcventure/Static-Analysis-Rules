static int
parse_netscreen_rec_hdr(wtap *wth, const char *line, char *cap_int, gboolean *cap_dir,
    union wtap_pseudo_header *pseudo_header _U_, int *err, gchar **err_info)
{
	int	sec;
	int	dsec, pkt_len;
	char	direction[2];

	if (sscanf(line, "%d.%d: %[a-z0-9/:.](%[io]) len=%d:",
		   &sec, &dsec, cap_int, direction, &pkt_len) != 5) {
		*err = WTAP_ERR_BAD_RECORD;
		*err_info = g_strdup("netscreen: Can't parse packet-header");
		return -1;
	}

	*cap_dir = (direction[0] == 'o' ? NETSCREEN_EGRESS : NETSCREEN_INGRESS);

	if (wth) {
		wth->phdr.ts.secs  = sec;
		wth->phdr.ts.nsecs = dsec * 100000000;
		wth->phdr.len = pkt_len;
	}

	return pkt_len;
}
