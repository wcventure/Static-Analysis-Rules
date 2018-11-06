static int
dissect_dvb_nit(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{

	guint       offset = 0;
	guint       ts_desc_len, desc_loop_len, ts_end;

	guint16     tsid;

	proto_item *ti;
	proto_tree *dvb_nit_tree;
	proto_item *tsi;
	proto_tree *dvb_nit_ts_tree;

	col_set_str(pinfo->cinfo, COL_INFO, "Network Information Table (NIT)");

	ti = proto_tree_add_item(tree, proto_dvb_nit, tvb, offset, -1, ENC_NA);
	dvb_nit_tree = proto_item_add_subtree(ti, ett_dvb_nit);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_nit_tree, NULL, NULL);

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_network_id,                 tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_reserved1,                  tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_version_number,             tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_current_next_indicator,     tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_section_number,             tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_last_section_number,        tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_reserved2,                  tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_network_descriptors_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	desc_loop_len = tvb_get_ntohs(tvb, offset) & DVB_NIT_NETWORK_DESCRIPTORS_LENGTH_MASK;
	offset += 2;

	offset += proto_mpeg_descriptor_loop_dissect(tvb, offset, desc_loop_len, dvb_nit_tree);

	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_reserved3,                    tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_nit_tree, hf_dvb_nit_transport_stream_loop_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	ts_end = offset + (tvb_get_ntohs(tvb, offset) & DVB_NIT_TRANSPORT_STREAM_LOOP_LENGTH_MASK);
	offset += 2;

	while (offset < ts_end) {
		tsid = tvb_get_ntohs(tvb, offset);
		ts_desc_len = 3 + (tvb_get_ntohs(tvb, offset + 4) & DVB_NIT_TRANSPORT_DESCRIPTORS_LENGTH_MASK);

		tsi = proto_tree_add_text(dvb_nit_tree, tvb, offset, ts_desc_len, "Stream ID=0x%04hx", tsid);
		dvb_nit_ts_tree = proto_item_add_subtree(tsi, ett_dvb_nit_ts);

		proto_tree_add_item(dvb_nit_ts_tree, hf_dvb_nit_transport_stream_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(dvb_nit_ts_tree, hf_dvb_nit_original_network_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(dvb_nit_ts_tree, hf_dvb_nit_reserved4,                    tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_nit_ts_tree, hf_dvb_nit_transport_descriptors_length, tvb, offset, 2, ENC_BIG_ENDIAN);
		desc_loop_len = tvb_get_ntohs(tvb, offset) & DVB_NIT_TRANSPORT_DESCRIPTORS_LENGTH_MASK;
		offset += 2;

		offset += proto_mpeg_descriptor_loop_dissect(tvb, offset, desc_loop_len, dvb_nit_ts_tree);
	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, dvb_nit_tree, 0, offset);

	proto_item_set_len(ti, offset);
	return offset;
}
