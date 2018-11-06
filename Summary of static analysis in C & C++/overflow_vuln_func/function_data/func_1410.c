static void
dissect_dvb_tdt(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint offset = 0;

	proto_item *ti;
	proto_tree *dvb_tdt_tree;

	nstime_t    utc_time;

	col_set_str(pinfo->cinfo, COL_INFO, "Time and Date Table (TDT)");

	ti = proto_tree_add_item(tree, proto_dvb_tdt, tvb, offset, -1, ENC_NA);
	dvb_tdt_tree = proto_item_add_subtree(ti, ett_dvb_tdt);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_tdt_tree, NULL, NULL);

	if (packet_mpeg_sect_mjd_to_utc_time(tvb, offset, &utc_time) < 0) {
		proto_tree_add_text(dvb_tdt_tree, tvb, offset, 5, "Unparseable time");
	} else {
		proto_tree_add_time_format(dvb_tdt_tree, hf_dvb_tdt_utc_time, tvb, offset, 5, &utc_time,
			"UTC Time : %s UTC", abs_time_to_str(&utc_time, ABSOLUTE_TIME_UTC, FALSE));
	}
	offset += 5;

	proto_item_set_len(ti, offset);
}
