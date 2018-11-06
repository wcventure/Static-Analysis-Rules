static void dissect_gnutella_query(tvbuff_t *tvb, guint offset, proto_tree *tree, guint size) {

	if(offset + size > tvb_length(tvb)) {
		proto_tree_add_item(tree,
			hf_gnutella_truncated,
			tvb,
			offset,
			size,
			FALSE);
		return;
	}

	proto_tree_add_item(tree,
		hf_gnutella_query_min_speed,
		tvb,
		offset + GNUTELLA_QUERY_SPEED_OFFSET,
		GNUTELLA_SHORT_LENGTH,
        TRUE);

    if (size > GNUTELLA_SHORT_LENGTH) {
        proto_tree_add_item(tree,
            hf_gnutella_query_search,
            tvb,
            offset + GNUTELLA_QUERY_SEARCH_OFFSET,
            size - GNUTELLA_SHORT_LENGTH,
            FALSE);
    }
    else {
        proto_tree_add_text(tree,
            tvb,
            offset + GNUTELLA_QUERY_SEARCH_OFFSET,
            0,
            "Missing data for Query Search.");
    }
}
