static gint
dissect_spotlight(tvbuff_t *tvb, proto_tree *tree, gint offset)
{
	guint encoding;
	gint i;
	gint querylen;
	gint toc_offset;
	gint toc_entries;
	guint64 toc_entry;
	
	proto_item *item_queries_data;
	proto_tree *sub_tree_queries;
	proto_item *item_toc;
	proto_tree *sub_tree_toc;

	if (strncmp(tvb_get_ephemeral_string(tvb, offset, 8), "md031234", 8) == 0)
		encoding = ENC_BIG_ENDIAN;
	else
		encoding = ENC_LITTLE_ENDIAN;
	proto_tree_add_text(tree,
			    tvb,
			    offset,
			    8,
			    "Endianess: %s",
			    encoding == ENC_BIG_ENDIAN ?
			    "Big Endian" : "Litte Endian");
	offset += 8;

	toc_offset = (gint)(spotlight_ntoh64(tvb, offset, encoding) >> 32) * 8 - 8;
	querylen = (gint)(spotlight_ntoh64(tvb, offset, encoding) & 0xffffffff) * 8 - 8;
	proto_tree_add_text(tree,
			    tvb,
			    offset,
			    8,
			    "ToC Offset: %u Bytes, Query length: %u Bytes",
			    toc_offset,
			    querylen);
	offset += 8;

	toc_offset += offset;
	toc_entries = (gint)(spotlight_ntoh64(tvb, toc_offset, encoding) & 0xffff) - 1;

	item_queries_data = proto_tree_add_text(tree,
						tvb,
						offset,
						toc_offset,
						"Spotlight RPC data");
	sub_tree_queries = proto_item_add_subtree(item_queries_data, ett_afp_spotlight_queries);

	/
	offset = spotlight_dissect_query_loop(tvb, sub_tree_queries, offset, SQ_CPX_TYPE_ARRAY, INT_MAX, toc_offset + 8, encoding);

	/
	offset = toc_offset;
	item_toc = proto_tree_add_text(tree,
				       tvb,
				       offset,
				       querylen - toc_offset,
				       "Complex types ToC (%u entries)",
				       toc_entries);
	sub_tree_toc = proto_item_add_subtree(item_toc, ett_afp_spotlight_toc);
	proto_tree_add_text(sub_tree_toc, tvb, offset, 2, "Number of entries (%u)", toc_entries);
	proto_tree_add_text(sub_tree_toc, tvb, offset + 2, 2, "unknown");
	proto_tree_add_text(sub_tree_toc, tvb, offset + 4, 4, "unknown");

	offset += 8;
	for (i = 0; i < toc_entries; i++, offset += 8) {
		toc_entry = spotlight_ntoh64(tvb, offset, encoding);
		if ((((toc_entry & 0xffff0000) >> 16) == SQ_CPX_TYPE_ARRAY)
		    || (((toc_entry & 0xffff0000) >> 16) == SQ_CPX_TYPE_DICT)) {
			proto_tree_add_text(sub_tree_toc,
					    tvb,
					    offset,
					    8,
					    "%u: count: %" G_GINT64_MODIFIER "u, type: %s, offset: %" G_GINT64_MODIFIER "u",
					    i+1,
					    toc_entry >> 32,
					    spotlight_get_cpx_qtype_string((toc_entry & 0xffff0000) >> 16),
					    (toc_entry & 0xffff) * 8);
		} else {
			proto_tree_add_text(sub_tree_toc,
					    tvb,
					    offset,
					    8,
					    "%u: unknown: 0x%08" G_GINT64_MODIFIER "x, type: %s, offset: %" G_GINT64_MODIFIER "u",
					    i+1,
					    toc_entry >> 32,
					    spotlight_get_cpx_qtype_string((toc_entry & 0xffff0000) >> 16),
					    (toc_entry & 0xffff) * 8);
		}
		
		
	}

	return offset;
}
