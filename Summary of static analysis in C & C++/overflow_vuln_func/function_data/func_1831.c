static void
dissect_ntp_std(tvbuff_t *tvb, proto_tree *ntp_tree, guint8 flags)
{
	proto_tree      *flags_tree;
	proto_item	*tf;
	guint8		stratum;
	guint8		ppoll;
	gint8		precision;
	double		rootdelay;
	double		rootdispersion;
	const guint8	*refid;
	guint32		refid_addr;
	const guint8	*reftime;
	const guint8	*org;
	const guint8	*rec;
	const guint8	*xmt;
	gchar		*buff;
	int		i;
	int		macofs;
	gint            maclen;

	tf = proto_tree_add_uint(ntp_tree, hf_ntp_flags, tvb, 0, 1, flags);

	/
	flags_tree = proto_item_add_subtree(tf, ett_ntp_flags);
	proto_tree_add_uint(flags_tree, hf_ntp_flags_li, tvb, 0, 1, flags);
	proto_tree_add_uint(flags_tree, hf_ntp_flags_vn, tvb, 0, 1, flags);
	proto_tree_add_uint(flags_tree, hf_ntp_flags_mode, tvb, 0, 1, flags);

	/
	stratum = tvb_get_guint8(tvb, 1);
	if (stratum == 0) {
		buff="Peer Clock Stratum: unspecified or unavailable (%u)";
	} else if (stratum == 1) {
		buff="Peer Clock Stratum: primary reference (%u)";
	} else if ((stratum >= 2) && (stratum <= 15)) {
		buff="Peer Clock Stratum: secondary reference (%u)";
	} else {
		buff="Peer Clock Stratum: reserved: %u";
	}
	proto_tree_add_uint_format(ntp_tree, hf_ntp_stratum, tvb, 1, 1,
				   stratum, buff, stratum);
	/
	ppoll = tvb_get_guint8(tvb, 2);
	proto_tree_add_uint_format(ntp_tree, hf_ntp_ppoll, tvb, 2, 1,
				   ppoll,
				   (((ppoll >= 4) && (ppoll <= 16)) ?
				   "Peer Polling Interval: %u (%u sec)" :
				   "Peer Polling Interval: invalid (%u)"),
				   ppoll,
				   1 << ppoll);

	/
	precision = tvb_get_guint8(tvb, 3);
	proto_tree_add_int_format(ntp_tree, hf_ntp_precision, tvb, 3, 1,
				   precision,
				   "Peer Clock Precision: %8.6f sec",
				   pow(2, precision));

	/
	rootdelay = ((gint16)tvb_get_ntohs(tvb, 4)) +
			(tvb_get_ntohs(tvb, 6) / 65536.0);
	proto_tree_add_double_format(ntp_tree, hf_ntp_rootdelay, tvb, 4, 4,
				   rootdelay,
				   "Root Delay: %9.4f sec",
				   rootdelay);

	/
	rootdispersion = ((gint16)tvb_get_ntohs(tvb, 8)) +
				(tvb_get_ntohs(tvb, 10) / 65536.0);
	proto_tree_add_double_format(ntp_tree, hf_ntp_rootdispersion, tvb, 8, 4,
				   rootdispersion,
				   "Clock Dispersion: %9.4f sec",
				   rootdispersion);

	/
	refid = tvb_get_ptr(tvb, 12, 4);
	if (stratum <= 1) {
		buff=ep_alloc(NTP_TS_SIZE);
		g_snprintf (buff, NTP_TS_SIZE, "Unindentified reference source '%.4s'",
			refid);
		for (i = 0; primary_sources[i].id; i++) {
			if (memcmp (refid, primary_sources[i].id, 4) == 0) {
				g_snprintf(buff, NTP_TS_SIZE, "%s",
					primary_sources[i].data);
				break;
			}
		}
	} else {
		int buffpos;
		buff = ep_alloc(NTP_TS_SIZE);
		refid_addr = tvb_get_ipv4(tvb, 12);
		buffpos = g_snprintf(buff, NTP_TS_SIZE, get_hostname (refid_addr));
		if (buffpos >= NTP_TS_SIZE) {
			buff[NTP_TS_SIZE-4]='.';
			buff[NTP_TS_SIZE-3]='.';
			buff[NTP_TS_SIZE-2]='.';
			buff[NTP_TS_SIZE-1]=0;
		}
	}
	proto_tree_add_bytes_format(ntp_tree, hf_ntp_refid, tvb, 12, 4,
				   refid,
				   "Reference Clock ID: %s", buff);

	/
	reftime = tvb_get_ptr(tvb, 16, 8);
	proto_tree_add_bytes_format(ntp_tree, hf_ntp_reftime, tvb, 16, 8,
				   reftime,
			           "Reference Clock Update Time: %s",
				   ntp_fmt_ts(reftime));

	/
	org = tvb_get_ptr(tvb, 24, 8);
	proto_tree_add_bytes_format(ntp_tree, hf_ntp_org, tvb, 24, 8,
				   org,
			           "Originate Time Stamp: %s",
				   ntp_fmt_ts(org));

	/
	rec = tvb_get_ptr(tvb, 32, 8);
	proto_tree_add_bytes_format(ntp_tree, hf_ntp_rec, tvb, 32, 8,
				   rec,
			           "Receive Time Stamp: %s",
				   ntp_fmt_ts(rec));

	/
	xmt = tvb_get_ptr(tvb, 40, 8);
	proto_tree_add_bytes_format(ntp_tree, hf_ntp_xmt, tvb, 40, 8,
				   xmt,
			           "Transmit Time Stamp: %s",
				   ntp_fmt_ts(xmt));

	/
	macofs = 48;
	while (tvb_reported_length_remaining(tvb, macofs) > (gint)MAX_MAC_LEN)
		macofs = dissect_ntp_ext(tvb, ntp_tree, macofs);

	/
	if (tvb_reported_length_remaining(tvb, macofs) >= 4)
		proto_tree_add_item(ntp_tree, hf_ntp_keyid, tvb, macofs, 4,
				    FALSE);
	macofs += 4;
	maclen = tvb_reported_length_remaining(tvb, macofs);
	if (maclen > 0)
		proto_tree_add_item(ntp_tree, hf_ntp_mac, tvb, macofs,
				    maclen, FALSE);
}
