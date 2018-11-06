static void
dissect_dvb_sdt(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint       offset = 0, length = 0;
	guint       descriptor_len, descriptor_end;
	guint16     svc_id;

	proto_item *ti;
	proto_tree *dvb_sdt_tree;
	proto_item *si;
	proto_tree *dvb_sdt_service_tree;

	/

	col_set_str(pinfo->cinfo, COL_INFO, "Service Description Table (SDT)");

	ti = proto_tree_add_item(tree, proto_dvb_sdt, tvb, offset, -1, ENC_NA);
	dvb_sdt_tree = proto_item_add_subtree(ti, ett_dvb_sdt);

	offset += packet_mpeg_sect_header(tvb, offset, dvb_sdt_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_transport_stream_id,    tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_reserved1,              tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_version_number,         tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_current_next_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_section_number,         tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_last_section_number,    tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_original_network_id,    tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(dvb_sdt_tree, hf_dvb_sdt_reserved2,              tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;


	if (offset >= length)
		return;

	/
	while (offset < length) {

		svc_id = tvb_get_ntohs(tvb, offset);
		si = proto_tree_add_text(dvb_sdt_tree, tvb, offset, 5, "Service 0x%04hx", svc_id);
		dvb_sdt_service_tree = proto_item_add_subtree(si, ett_dvb_sdt_service);

		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_service_id,                 tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_reserved3,                  tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_eit_schedule_flag,          tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_eit_present_following_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset += 1;

		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_running_status,             tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_free_ca_mode,               tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(dvb_sdt_service_tree, hf_dvb_sdt_descriptors_loop_length,    tvb, offset, 2, ENC_BIG_ENDIAN);
		descriptor_len = tvb_get_ntohs(tvb, offset) & DVB_SDT_DESCRIPTORS_LOOP_LENGTH_MASK;
		offset += 2;

		descriptor_end = offset + descriptor_len;
		while (offset < descriptor_end)
			offset += proto_mpeg_descriptor_dissect(tvb, offset, dvb_sdt_service_tree);

	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, dvb_sdt_tree, 0, offset);
	proto_item_set_len(ti, offset);
}
