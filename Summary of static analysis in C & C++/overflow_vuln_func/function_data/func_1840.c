static void dissect_gnutella_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) {

	proto_item *ti, *hi, *pi;
	proto_tree *gnutella_tree = NULL;
	proto_tree *gnutella_header_tree, *gnutella_pong_tree;
	proto_tree *gnutella_queryhit_tree, *gnutella_push_tree;
	proto_tree *gnutella_query_tree;
	guint8 payload_descriptor;
	guint32 size = 0;
	char *payload_descriptor_text;

	if (tree) {
		ti = proto_tree_add_item(tree,
			proto_gnutella,
			tvb,
			0,
			-1,
			FALSE);
		gnutella_tree = proto_item_add_subtree(ti, ett_gnutella);

		size = tvb_get_letohl(
			tvb,
			GNUTELLA_HEADER_SIZE_OFFSET);
	}

	payload_descriptor = tvb_get_guint8(
		tvb,
		GNUTELLA_HEADER_PAYLOAD_OFFSET);

	switch(payload_descriptor) {
		case GNUTELLA_PING:
			payload_descriptor_text = GNUTELLA_PING_NAME;
			break;
		case GNUTELLA_PONG:
			payload_descriptor_text = GNUTELLA_PONG_NAME;
			break;
		case GNUTELLA_PUSH:
			payload_descriptor_text = GNUTELLA_PUSH_NAME;
			break;
		case GNUTELLA_QUERY:
			payload_descriptor_text = GNUTELLA_QUERY_NAME;
			break;
		case GNUTELLA_QUERYHIT:
			payload_descriptor_text = GNUTELLA_QUERYHIT_NAME;
			break;
		default:
			payload_descriptor_text = GNUTELLA_UNKNOWN_NAME;
			break;
	}

	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_sep_fstr(pinfo->cinfo, COL_INFO, NULL, "%s",
		    payload_descriptor_text);

	if (tree) {
		hi = proto_tree_add_item(gnutella_tree,
			hf_gnutella_header,
			tvb,
			0,
			GNUTELLA_HEADER_LENGTH,
			FALSE);
		gnutella_header_tree = proto_item_add_subtree(hi, ett_gnutella);

		proto_tree_add_item(gnutella_header_tree,
			hf_gnutella_header_id,
			tvb,
			GNUTELLA_HEADER_ID_OFFSET,
			GNUTELLA_SERVENT_ID_LENGTH,
			FALSE);

		proto_tree_add_uint_format(gnutella_header_tree,
			hf_gnutella_header_payload,
			tvb,
			GNUTELLA_HEADER_PAYLOAD_OFFSET,
			GNUTELLA_BYTE_LENGTH,
			payload_descriptor,
			"Payload: %i (%s)",
			payload_descriptor,
			payload_descriptor_text);

		proto_tree_add_item(gnutella_header_tree,
			hf_gnutella_header_ttl,
			tvb,
			GNUTELLA_HEADER_TTL_OFFSET,
			GNUTELLA_BYTE_LENGTH,
			FALSE);

		proto_tree_add_item(gnutella_header_tree,
			hf_gnutella_header_hops,
			tvb,
			GNUTELLA_HEADER_HOPS_OFFSET,
			GNUTELLA_BYTE_LENGTH,
			FALSE);

		proto_tree_add_uint(gnutella_header_tree,
			hf_gnutella_header_size,
			tvb,
			GNUTELLA_HEADER_SIZE_OFFSET,
			GNUTELLA_LONG_LENGTH,
			size);

		if (size > 0) {
			switch(payload_descriptor) {
			case GNUTELLA_PONG:
				pi = proto_tree_add_item(
					gnutella_header_tree,
					hf_gnutella_pong_payload,
					tvb,
					GNUTELLA_HEADER_LENGTH,
					size,
					FALSE);
				gnutella_pong_tree = proto_item_add_subtree(
					pi,
					ett_gnutella);
				dissect_gnutella_pong(
					tvb,
					GNUTELLA_HEADER_LENGTH,
					gnutella_pong_tree,
					size);
				break;
			case GNUTELLA_PUSH:
				pi = proto_tree_add_item(
					gnutella_header_tree,
					hf_gnutella_push_payload,
					tvb,
					GNUTELLA_HEADER_LENGTH,
					size,
					FALSE);
				gnutella_push_tree = proto_item_add_subtree(
					pi,
					ett_gnutella);
				dissect_gnutella_push(
					tvb,
					GNUTELLA_HEADER_LENGTH,
					gnutella_push_tree,
					size);
				break;
			case GNUTELLA_QUERY:
				pi = proto_tree_add_item(
					gnutella_header_tree,
					hf_gnutella_query_payload,
					tvb,
					GNUTELLA_HEADER_LENGTH,
					size,
					FALSE);
				gnutella_query_tree = proto_item_add_subtree(
					pi,
					ett_gnutella);
				dissect_gnutella_query(
					tvb,
					GNUTELLA_HEADER_LENGTH,
					gnutella_query_tree,
					size);
				break;
			case GNUTELLA_QUERYHIT:
				pi = proto_tree_add_item(
					gnutella_header_tree,
					hf_gnutella_queryhit_payload,
					tvb,
					GNUTELLA_HEADER_LENGTH,
					size,
					FALSE);
				gnutella_queryhit_tree = proto_item_add_subtree(
					pi,
					ett_gnutella);
				dissect_gnutella_queryhit(
					tvb,
					GNUTELLA_HEADER_LENGTH,
					gnutella_queryhit_tree,
					size);
				break;
			}
		}
	}

}
