static void
dissect_mpeg_sect(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	gint     offset           = 0;
	guint    section_length   = 0;
	gboolean syntax_indicator = FALSE;
	guint8   table_id;

	proto_item *ti;
	proto_tree *mpeg_sect_tree;

	table_id = tvb_get_guint8(tvb, offset);

	/
	if (dissector_try_uint(mpeg_sect_tid_dissector_table, table_id, tvb, pinfo, tree))
		return;

	/
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "MPEG SECT");
	col_add_fstr(pinfo->cinfo, COL_INFO, "Table ID 0x%02x", table_id);

	ti = proto_tree_add_item(tree, proto_mpeg_sect, tvb, offset, -1, ENC_NA);
	mpeg_sect_tree = proto_item_add_subtree(ti, ett_mpeg_sect);

	proto_item_append_text(ti, " Table_ID=0x%02x", table_id);

	packet_mpeg_sect_header(tvb, offset, mpeg_sect_tree,
				&section_length, &syntax_indicator);

	if (syntax_indicator)
		packet_mpeg_sect_crc(tvb, pinfo, mpeg_sect_tree, 0, (section_length-1));
}
