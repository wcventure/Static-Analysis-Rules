static void
dissect_dvb_tot(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint       offset = 0;
	guint       descriptor_len, descriptor_end;

	proto_item *ti;
	proto_tree *dvb_tot_tree;

	nstime_t    utc_time;

	col_set_str(pinfo->cinfo, COL_INFO, "Time Offset Table (TOT)");

	ti = proto_tree_add_item(tree, proto_dvb_tot, tvb, offset, -1, ENC_NA);
	dvb_tot_tree = proto_item_add_subtree(ti, ett_dvb_tot);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_tot_tree, NULL, NULL);

	if (packet_mpeg_sect_mjd_to_utc_time(tvb, offset, &utc_time) < 0) {
		proto_tree_add_text(dvb_tot_tree, tvb, offset, 5, "UTC Time : Unparseable time");
	} else {
		proto_tree_add_time_format(dvb_tot_tree, hf_dvb_tot_utc_time, tvb, offset, 5, &utc_time,
			"UTC Time : %s UTC", abs_time_to_str(&utc_time, ABSOLUTE_TIME_UTC, FALSE));
	}

	offset += 5;

	descriptor_len = tvb_get_ntohs(tvb, offset) & DVB_TOT_DESCRIPTORS_LOOP_LENGTH_MASK;
	proto_tree_add_item(dvb_tot_tree, hf_dvb_tot_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_tot_tree, hf_dvb_tot_descriptors_loop_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	descriptor_end = offset + descriptor_len;
	while (offset < descriptor_end)
		offset += proto_mpeg_descriptor_dissect(tvb, offset, dvb_tot_tree);

	offset += packet_mpeg_sect_crc(tvb, pinfo, dvb_tot_tree, 0, offset);
	proto_item_set_len(ti, offset);
}
