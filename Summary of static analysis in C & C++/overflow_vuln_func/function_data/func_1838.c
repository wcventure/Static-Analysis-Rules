static void dissect_gnutella_push(tvbuff_t *tvb, guint offset, proto_tree *tree, guint size) {

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
		hf_gnutella_push_servent_id,
		tvb,
		offset + GNUTELLA_PUSH_SERVENT_ID_OFFSET,
		GNUTELLA_SERVENT_ID_LENGTH,
		FALSE);

	proto_tree_add_item(tree,
		hf_gnutella_push_index,
		tvb,
		offset + GNUTELLA_PUSH_INDEX_OFFSET,
		GNUTELLA_LONG_LENGTH,
		TRUE);

	proto_tree_add_item(tree,
		hf_gnutella_push_ip,
		tvb,
		offset + GNUTELLA_PUSH_IP_OFFSET,
		GNUTELLA_IP_LENGTH,
		FALSE);

	proto_tree_add_item(tree,
		hf_gnutella_push_port,
		tvb,
		offset + GNUTELLA_PUSH_PORT_OFFSET,
		GNUTELLA_PORT_LENGTH,
		TRUE);

}
