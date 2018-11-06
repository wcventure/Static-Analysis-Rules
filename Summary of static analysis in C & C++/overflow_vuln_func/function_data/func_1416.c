static void
dissect_mpeg_ca(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

	guint offset = 0, length = 0;

	proto_item *ti;
	proto_tree *mpeg_ca_tree;

	/

	col_set_str(pinfo->cinfo, COL_INFO, "Conditional Access Table (CA)");

	ti = proto_tree_add_item(tree, proto_mpeg_ca, tvb, offset, -1, ENC_NA);
	mpeg_ca_tree = proto_item_add_subtree(ti, ett_mpeg_ca);

	offset += packet_mpeg_sect_header(tvb, offset, mpeg_ca_tree, &length, NULL);
	length -= 4;

	proto_tree_add_item(mpeg_ca_tree, hf_mpeg_ca_reserved, tvb, offset, 3, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_ca_tree, hf_mpeg_ca_version_number, tvb, offset, 3, ENC_BIG_ENDIAN);
	proto_tree_add_item(mpeg_ca_tree, hf_mpeg_ca_current_next_indicator, tvb, offset, 3, ENC_BIG_ENDIAN);
	offset += 3;

	proto_tree_add_item(mpeg_ca_tree, hf_mpeg_ca_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(mpeg_ca_tree, hf_mpeg_ca_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	/
	while (offset < length)
		offset += proto_mpeg_descriptor_dissect(tvb, offset, mpeg_ca_tree);

	offset += packet_mpeg_sect_crc(tvb, pinfo, mpeg_ca_tree, 0, offset);

	proto_item_set_len(ti, offset);
}
