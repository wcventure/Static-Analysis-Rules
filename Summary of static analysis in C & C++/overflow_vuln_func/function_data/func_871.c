static void
dissect_ntp(const u_char *pd, int offset, frame_data *fd, proto_tree *tree)
{
	proto_tree      *ntp_tree, *flags_tree;
	proto_item	*ti, *tf;
	struct ntp_packet *pkt;
	gchar buff[NTP_TS_SIZE];
	int i;

	OLD_CHECK_DISPLAY_AS_DATA(proto_ntp, pd, offset, fd, tree);

	/
	if ( !BYTES_ARE_IN_FRAME(offset, 48) ) /
		return;

	pkt = (struct ntp_packet *) &pd[offset];
	
	if (check_col(fd, COL_PROTOCOL))
		col_set_str(fd, COL_PROTOCOL, "NTP");

	if (check_col(fd, COL_INFO))
		col_set_str(fd, COL_INFO, "NTP");

	if (tree) {
		/
		ti = proto_tree_add_item(tree, proto_ntp, NullTVB, offset, END_OF_FRAME, FALSE);
		ntp_tree = proto_item_add_subtree(ti, ett_ntp);
		tf = proto_tree_add_bytes(ntp_tree, hf_ntp_flags, NullTVB, offset, 1, pkt->flags);

		/
		flags_tree = proto_item_add_subtree(tf, ett_ntp_flags);
		proto_tree_add_uint_format(flags_tree, hf_ntp_flags_li, NullTVB, offset, 1,
					   *pkt->flags & NTP_LI_MASK,
					   decode_enumerated_bitfield(*pkt->flags, NTP_LI_MASK,
				           sizeof(pkt->flags) * 8, li_types, "Leap Indicator: %s"));
		proto_tree_add_uint_format(flags_tree, hf_ntp_flags_vn, NullTVB, offset, 1,
					   *pkt->flags & NTP_VN_MASK,
					   decode_enumerated_bitfield(*pkt->flags, NTP_VN_MASK,
				           sizeof(pkt->flags) * 8, ver_nums, "Version number: %s"));
		proto_tree_add_uint_format(flags_tree, hf_ntp_flags_mode, NullTVB, offset, 1,
					   *pkt->flags & NTP_MODE_MASK,
					   decode_enumerated_bitfield(*pkt->flags, NTP_MODE_MASK,
				           sizeof(pkt->flags) * 8, mode_types, "Mode: %s"));

		/
		if (*pkt->stratum == 0) {
			strcpy (buff, "Peer Clock Stratum: unspecified or unavailable (%d)");
		} else if (*pkt->stratum == 1) {
			strcpy (buff, "Peer Clock Stratum: primary reference (%d)");
		} else if ((*pkt->stratum >= 2) && (*pkt->stratum <= 15)) {
			strcpy (buff, "Peer Clock Stratum: secondary reference (%d)");
		} else {
			strcpy (buff, "Peer Clock Stratum: reserved: %d");
		}
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_stratum, NullTVB, offset+1, 1, pkt->stratum,
					   buff, (int) *pkt->stratum);
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_ppoll, NullTVB, offset+2, 1, pkt->ppoll,
					   (((*pkt->ppoll >= 4) && (*pkt->ppoll <= 16)) ? 
					   "Peer Pooling Interval: %d (%d sec)" :
					   "Peer Pooling Interval: invalid (%d)"), (int) *pkt->ppoll,
					   1 << *pkt->ppoll);
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_precision, NullTVB, offset+3, 1, pkt->precision,
					   "Peer Clock Precision: %8.6f sec", pow(2, *pkt->precision));
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_rootdelay, NullTVB, offset+4, 4, pkt->rootdelay,
					   "Root Delay: %9.4f sec",
					   ((gint32) pntohs(pkt->rootdelay)) +
					   pntohs(pkt->rootdelay + 2) / 65536.0);
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_rootdispersion, NullTVB, offset+8, 4, pkt->rootdispersion,
					   "Clock Dispersion: %9.4f sec",
					   ((gint32) pntohs(pkt->rootdispersion)) +
					   pntohs(pkt->rootdispersion + 2) / 65536.0);
		/
		if (*pkt->stratum <= 1) {
			snprintf (buff, sizeof buff,
			    "Unindentified reference source '%.4s'",
			    pkt->refid); 
			for (i = 0; primary_sources[i].id; i++) {
				if (memcmp (pkt->refid, primary_sources[i].id,
				    4) == 0) {
					strcpy (buff, primary_sources[i].data);
					break;
				}
			}
		} else
			strcpy (buff, get_hostname (pntohl(pkt->refid)));
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_refid, NullTVB, offset+12, 4, pkt->refid,
					   "Reference Clock ID: %s", buff);
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_reftime, NullTVB, offset+16, 8, pkt->reftime,
				           "Reference Clock Update Time: %s", 
					   ntp_fmt_ts(pkt->reftime, buff));
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_org, NullTVB, offset+24, 8, pkt->org,
				           "Originate Time Stamp: %s", 
					   ntp_fmt_ts(pkt->org, buff));
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_rec, NullTVB, offset+32, 8, pkt->rec,
				           "Receive Time Stamp: %s", 
					   ntp_fmt_ts(pkt->rec, buff));
		/
		proto_tree_add_bytes_format(ntp_tree, hf_ntp_xmt, NullTVB, offset+40, 8, pkt->xmt,
				           "Transmit Time Stamp: %s", 
					   ntp_fmt_ts(pkt->xmt, buff));

		/
		if ( BYTES_ARE_IN_FRAME(offset, 50) )
			proto_tree_add_bytes(ntp_tree, hf_ntp_keyid, NullTVB, offset+48, 4, pkt->keyid);
		if ( BYTES_ARE_IN_FRAME(offset, 53) )
			proto_tree_add_bytes(ntp_tree, hf_ntp_mac, NullTVB, offset+52, END_OF_FRAME, pkt->mac);

	}
}
