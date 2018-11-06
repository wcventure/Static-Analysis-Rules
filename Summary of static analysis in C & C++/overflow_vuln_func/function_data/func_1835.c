static void dissect_gnutella_pong(tvbuff_t *tvb, guint offset, proto_tree *tree, guint size) {

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
		hf_gnutella_pong_port,
		tvb,
		offset + GNUTELLA_PONG_PORT_OFFSET,
		GNUTELLA_PORT_LENGTH,
		TRUE);

	proto_tree_add_item(tree,
		hf_gnutella_pong_ip,
		tvb,
		offset + GNUTELLA_PONG_IP_OFFSET,
		GNUTELLA_IP_LENGTH,
        FALSE);

	proto_tree_add_item(tree,
		hf_gnutella_pong_files,
		tvb,
		offset + GNUTELLA_PONG_FILES_OFFSET,
		GNUTELLA_LONG_LENGTH,
		TRUE);

	proto_tree_add_item(tree,
		hf_gnutella_pong_kbytes,
		tvb,
		offset + GNUTELLA_PONG_KBYTES_OFFSET,
		GNUTELLA_LONG_LENGTH,
		TRUE);

}
