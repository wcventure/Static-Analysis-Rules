static void dissect_gnutella_queryhit(tvbuff_t *tvb, guint offset, proto_tree *tree, guint size) {

	proto_tree *qhi, *hit_tree;
	int hit_count, i;
	int hit_offset;
	int name_length, extra_length;
	int idx_at_offset, size_at_offset;
	int servent_id_at_offset;
	int name_at_offset, extra_at_offset;
	int cur_char, remaining, used;

	if(offset + size > tvb_length(tvb)) {
		proto_tree_add_item(tree,
			hf_gnutella_truncated,
			tvb,
			offset,
			size,
			FALSE);
		return;
	}

	hit_count = tvb_get_guint8(tvb, offset + GNUTELLA_QUERYHIT_COUNT_OFFSET);

	proto_tree_add_uint(tree,
		hf_gnutella_queryhit_count,
		tvb,
		offset + GNUTELLA_QUERYHIT_COUNT_OFFSET,
		GNUTELLA_BYTE_LENGTH,
		hit_count);

	proto_tree_add_item(tree,
		hf_gnutella_queryhit_port,
		tvb,
		offset + GNUTELLA_QUERYHIT_PORT_OFFSET,
		GNUTELLA_PORT_LENGTH,
		TRUE);

	proto_tree_add_item(tree,
		hf_gnutella_queryhit_ip,
		tvb,
		offset + GNUTELLA_QUERYHIT_IP_OFFSET,
		GNUTELLA_IP_LENGTH,
		FALSE);

	proto_tree_add_item(tree,
		hf_gnutella_queryhit_speed,
		tvb,
		offset + GNUTELLA_QUERYHIT_SPEED_OFFSET,
		GNUTELLA_LONG_LENGTH,
		TRUE);

	hit_offset = offset + GNUTELLA_QUERYHIT_FIRST_HIT_OFFSET;

	for(i = 0; i < hit_count; i++) {
		idx_at_offset  = hit_offset;
		size_at_offset = hit_offset + GNUTELLA_QUERYHIT_HIT_SIZE_OFFSET;

		hit_offset += (GNUTELLA_LONG_LENGTH * 2);

		name_length  = 0;
		extra_length = 0;

		name_at_offset = hit_offset;

		while(hit_offset - offset < size) {
			cur_char = tvb_get_guint8(tvb, hit_offset);
			if(cur_char == '\0')
				break;

			hit_offset++;
			name_length++;
		}

		hit_offset++;

		extra_at_offset = hit_offset;

		while(hit_offset - offset < size) {
			cur_char = tvb_get_guint8(tvb, hit_offset);
			if(cur_char == '\0')
				break;

			hit_offset++;
			extra_length++;
		}

		hit_offset++;

		qhi = proto_tree_add_item(tree,
			hf_gnutella_queryhit_hit,
			tvb,
			idx_at_offset,
			(GNUTELLA_LONG_LENGTH * 2) +
			name_length + extra_length +
			GNUTELLA_QUERYHIT_END_OF_STRING_LENGTH,
			FALSE);

		hit_tree = proto_item_add_subtree(qhi, ett_gnutella);

		proto_tree_add_item(hit_tree,
			hf_gnutella_queryhit_hit_index,
			tvb,
			idx_at_offset,
			GNUTELLA_LONG_LENGTH,
			TRUE);

		proto_tree_add_item(hit_tree,
			hf_gnutella_queryhit_hit_size,
			tvb,
			size_at_offset,
			GNUTELLA_LONG_LENGTH,
			TRUE);

		proto_tree_add_item(hit_tree,
			hf_gnutella_queryhit_hit_name,
			tvb,
			name_at_offset,
			name_length,
			FALSE);

		if(extra_length) {
			proto_tree_add_item(hit_tree,
				hf_gnutella_queryhit_hit_extra,
				tvb,
				extra_at_offset,
				extra_length,
				FALSE);
		}
	}

	used = hit_offset - offset;
	remaining = size - used;

	if(remaining > GNUTELLA_SERVENT_ID_LENGTH) {
		servent_id_at_offset = hit_offset + remaining - GNUTELLA_SERVENT_ID_LENGTH;

		proto_tree_add_item(tree,
			hf_gnutella_queryhit_extra,
			tvb,
			hit_offset,
			servent_id_at_offset - hit_offset,
			FALSE);
	}
	else {
		servent_id_at_offset = hit_offset;
	}

	proto_tree_add_item(tree,
		hf_gnutella_queryhit_servent_id,
		tvb,
		servent_id_at_offset,
		GNUTELLA_SERVENT_ID_LENGTH,
		FALSE);

}
