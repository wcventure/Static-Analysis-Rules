static void
dissect_dvb_eit(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint       offset = 0, length = 0;
	guint       descriptor_len, descriptor_end;
	guint16     evt_id;

	proto_item *ti;
	proto_tree *dvb_eit_tree;
	proto_item *ei;
	proto_tree *dvb_eit_event_tree;
	proto_item *duration_item;

	nstime_t    start_time;

	col_set_str(pinfo->cinfo, COL_INFO, "Event Information Table (EIT)");

	ti = proto_tree_add_item(tree, proto_dvb_eit, tvb, offset, -1, ENC_NA);
	dvb_eit_tree = proto_item_add_subtree(ti, ett_dvb_eit);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_eit_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_service_id,                  tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_reserved,                    tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_version_number,              tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_current_next_indicator,      tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_section_number,              tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_last_section_number,         tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_transport_stream_id,         tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_original_network_id,         tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_segment_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_eit_tree, hf_dvb_eit_last_table_id, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	if (offset >= length) {
		packet_mpeg_sect_crc(tvb, pinfo, dvb_eit_tree, 0, offset);

		return;
	}

	/
	while (offset < length) {

		evt_id = tvb_get_ntohs(tvb, offset);
		ei = proto_tree_add_text(dvb_eit_tree, tvb, offset, 12, "Event 0x%04hx", evt_id);
		dvb_eit_event_tree = proto_item_add_subtree(ei, ett_dvb_eit_event);

		proto_tree_add_item(dvb_eit_event_tree, hf_dvb_eit_event_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		if (tvb_memeql(tvb, offset, "\xFF\xFF\xFF\xFF\xFF", 5)) {
			if (packet_mpeg_sect_mjd_to_utc_time(tvb, offset, &start_time) < 0) {
				proto_tree_add_text(tree, tvb, offset, 5, "Unparseable time");
			} else {
				proto_tree_add_time_format(dvb_eit_event_tree, hf_dvb_eit_start_time, tvb, offset,
					5, &start_time,
					"Start Time: %s UTC", abs_time_to_str(&start_time, ABSOLUTE_TIME_UTC, FALSE));
			}
		} else {
			proto_tree_add_text(tree, tvb, offset, 5, "Start Time: Undefined (0xFFFFFFFFFF)");
		}
		offset += 5;

		duration_item = proto_tree_add_item(dvb_eit_event_tree, hf_dvb_eit_duration, tvb, offset, 3, ENC_BIG_ENDIAN);
		proto_item_append_text(duration_item, " (%02u:%02u:%02u)",
			MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, offset)),
			MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, offset + 1)),
			MPEG_SECT_BCD44_TO_DEC(tvb_get_guint8(tvb, offset + 2)));
		offset += 3;

		proto_tree_add_item(dvb_eit_event_tree, hf_dvb_eit_running_status,          tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_eit_event_tree, hf_dvb_eit_free_ca_mode,            tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_eit_event_tree, hf_dvb_eit_descriptors_loop_length, tvb, offset, 2, ENC_BIG_ENDIAN);
		descriptor_len = tvb_get_ntohs(tvb, offset) & DVB_EIT_DESCRIPTORS_LOOP_LENGTH_MASK;
		offset += 2;

		descriptor_end = offset + descriptor_len;
		while (offset < descriptor_end)
			offset += proto_mpeg_descriptor_dissect(tvb, offset, dvb_eit_event_tree);

	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, dvb_eit_tree, 0, offset);
	proto_item_set_len(ti, offset);
}
