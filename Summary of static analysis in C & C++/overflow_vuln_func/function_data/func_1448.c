static void
proto_mpeg_descriptor_dissect_local_time_offset(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint    end = offset + len;
	guint16  time_offset;
	nstime_t time_of_change;

	while (offset < end) {
		proto_tree_add_item(tree, hf_mpeg_descr_local_time_offset_country_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_local_time_offset_region_id, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_local_time_offset_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_local_time_offset_polarity, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		time_offset = tvb_get_ntohs(tvb, offset);
		proto_tree_add_string_format_value(tree, hf_mpeg_descr_local_time_offset_offset, tvb, offset, 2,
			"Local Time Offset", "%02u:%02u",
			MPEG_SECT_BCD44_TO_DEC(time_offset >> 8),
			MPEG_SECT_BCD44_TO_DEC(time_offset));
		offset += 2;


		if (packet_mpeg_sect_mjd_to_utc_time(tvb, offset, &time_of_change) < 0) {
			proto_tree_add_text(tree, tvb, offset, 5, "Time of Change : Unparseable time");
		} else {
			proto_tree_add_time_format(tree, hf_mpeg_descr_local_time_offset_time_of_change, tvb, offset, 5,
			&time_of_change, "Time of Change : %s UTC",
			abs_time_to_str(&time_of_change, ABSOLUTE_TIME_UTC, FALSE));
		}
		offset += 5;

		time_offset = tvb_get_ntohs(tvb, offset);
		proto_tree_add_string_format_value(tree, hf_mpeg_descr_local_time_offset_next_time_offset, tvb, offset, 2,
			"Next Time Offset", "%02u:%02u",
			MPEG_SECT_BCD44_TO_DEC(time_offset >> 8),
			MPEG_SECT_BCD44_TO_DEC(time_offset));
		offset += 2;
	}
}
