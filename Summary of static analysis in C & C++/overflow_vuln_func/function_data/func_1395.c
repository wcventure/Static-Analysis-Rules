static void
dissect_dvb_bat(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint offset = 0, length = 0, descriptor_end = 0, ts_loop_end = 0;
	guint16 ts_id = 0, descriptor_len = 0, ts_loop_len = 0;

	proto_item *ti;
	proto_tree *dvb_bat_tree;
	proto_item *tsi;
	proto_tree *transport_stream_tree;

	col_clear(pinfo->cinfo, COL_INFO);
	col_set_str(pinfo->cinfo, COL_INFO, "Bouquet Association Table (BAT)");

	ti = proto_tree_add_item(tree, proto_dvb_bat, tvb, offset, -1, ENC_NA);
	dvb_bat_tree = proto_item_add_subtree(ti, ett_dvb_bat);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_bat_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_bouquet_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_version_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_current_next_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	descriptor_len = tvb_get_ntohs(tvb, offset) & DVB_BAT_BOUQUET_DESCRIPTORS_LENGTH_MASK;
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_reserved2, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_bouquet_descriptors_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	descriptor_end = offset + descriptor_len;
	while (offset < descriptor_end)
		offset += proto_mpeg_descriptor_dissect(tvb, offset, dvb_bat_tree);

	ts_loop_len = tvb_get_ntohs(tvb, offset) & DVB_BAT_TRANSPORT_STREAM_LOOP_LENGTH_MASK;
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_reserved3, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_bat_tree, hf_dvb_bat_transport_stream_loop_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	ts_loop_end = offset + ts_loop_len;
	while (offset < ts_loop_end) {
		ts_id = tvb_get_ntohs(tvb, offset);
		descriptor_len = tvb_get_ntohs(tvb, offset + 4) & DVB_BAT_TRANSPORT_DESCRIPTORS_LENGTH_MASK;

		tsi = proto_tree_add_text(dvb_bat_tree, tvb, offset, 6 + descriptor_len, "Transport Stream 0x%04x", ts_id);
		transport_stream_tree = proto_item_add_subtree(tsi, ett_dvb_bat_transport_stream);

		proto_tree_add_item(transport_stream_tree, hf_dvb_bat_transport_stream_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(transport_stream_tree, hf_dvb_bat_original_network_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(transport_stream_tree, hf_dvb_bat_reserved4, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(transport_stream_tree, hf_dvb_bat_transport_descriptors_length, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		descriptor_end = offset + descriptor_len;
		while (offset < descriptor_end)
			offset += proto_mpeg_descriptor_dissect(tvb, offset, transport_stream_tree);
	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, dvb_bat_tree, 0, offset);
	proto_item_set_len(ti, offset);
}
