static void
dissect_mpeg_pat(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint offset = 0, length = 0;
	guint16 prog_num = 0, prog_pid;

	proto_item *ti;
	proto_tree *mpeg_pat_tree;
	proto_item *pi;
	proto_tree *mpeg_pat_prog_tree;

	/

	col_clear(pinfo->cinfo, COL_INFO);
	col_set_str(pinfo->cinfo, COL_INFO, "Program Association Table (PAT)");

	ti = proto_tree_add_item(tree, proto_mpeg_pat, tvb, offset, -1, ENC_NA);
	mpeg_pat_tree = proto_item_add_subtree(ti, ett_mpeg_pat);

	offset += packet_mpeg_sect_header(tvb, offset, mpeg_pat_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_transport_stream_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_version_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_current_next_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_pat_tree, hf_mpeg_pat_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (offset >= length)
		return;


	/
	while (offset < length) {

		prog_num = tvb_get_ntohs(tvb, offset);
		prog_pid = tvb_get_ntohs(tvb, offset + 2) & MPEG_PAT_PROGRAM_MAP_PID_MASK;

		pi = proto_tree_add_text(mpeg_pat_tree, tvb, offset, 4, "Program 0x%04hx -> PID 0x%04hx", prog_num, prog_pid);
		mpeg_pat_prog_tree = proto_item_add_subtree(pi, ett_mpeg_pat_prog);

		proto_tree_add_item(mpeg_pat_prog_tree, hf_mpeg_pat_program_number, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(mpeg_pat_prog_tree, hf_mpeg_pat_program_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(mpeg_pat_prog_tree, hf_mpeg_pat_program_map_pid, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

	}

	offset += packet_mpeg_sect_crc(tvb, pinfo, mpeg_pat_tree, 0, offset);
	proto_item_set_len(ti, offset);
}
