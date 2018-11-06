static int
dissect_mpeg_pmt(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{

	guint   offset = 0, length = 0;
	guint   descriptor_end, prog_info_len, es_info_len;
	guint16 pid;

	proto_item *ti;
	proto_tree *mpeg_pmt_tree;
	proto_item *si;
	proto_tree *mpeg_pmt_stream_tree;

	/

	col_set_str(pinfo->cinfo, COL_INFO, "Program Map Table (PMT)");

	ti = proto_tree_add_item(tree, proto_mpeg_pmt, tvb, offset, -1, ENC_NA);
	mpeg_pmt_tree = proto_item_add_subtree(ti, ett_mpeg_pmt);

	offset += packet_mpeg_sect_header(tvb, offset, mpeg_pmt_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_program_number, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_version_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_current_next_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_reserved2, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_pcr_pid, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	prog_info_len = tvb_get_ntohs(tvb, offset) & MPEG_PMT_PROGRAM_INFO_LENGTH_MASK;
	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_reserved3, tvb, offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pmt_tree, hf_mpeg_pmt_program_info_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	descriptor_end = offset + prog_info_len;
	while (offset < descriptor_end)
		offset += proto_mpeg_descriptor_dissect(tvb, offset, mpeg_pmt_tree);

	while (offset < length) {

		pid = tvb_get_ntohs(tvb, offset + 1) & MPEG_PMT_STREAM_ELEMENTARY_PID_MASK;
		es_info_len = tvb_get_ntohs(tvb, offset + 3) & MPEG_PMT_STREAM_ES_INFO_LENGTH_MASK;

		si = proto_tree_add_text(mpeg_pmt_tree, tvb, offset, 5 + es_info_len, "Stream PID=0x%04hx", pid);
		mpeg_pmt_stream_tree = proto_item_add_subtree(si, ett_mpeg_pmt_stream);

		proto_tree_add_item(mpeg_pmt_stream_tree, hf_mpeg_pmt_stream_type,		tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(mpeg_pmt_stream_tree, hf_mpeg_pmt_stream_reserved1,		tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(mpeg_pmt_stream_tree, hf_mpeg_pmt_stream_elementary_pid,	tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(mpeg_pmt_stream_tree, hf_mpeg_pmt_stream_reserved2,		tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(mpeg_pmt_stream_tree, hf_mpeg_pmt_stream_es_info_length,	tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		descriptor_end = offset + es_info_len;
		while (offset < descriptor_end)
			offset += proto_mpeg_descriptor_dissect(tvb, offset, mpeg_pmt_stream_tree);

	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, mpeg_pmt_tree, 0, offset);

	proto_item_set_len(ti, offset);
	return offset;
}
