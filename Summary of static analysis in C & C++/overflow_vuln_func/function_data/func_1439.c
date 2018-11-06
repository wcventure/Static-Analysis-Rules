static void
proto_mpeg_descriptor_dissect_linkage(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{

	guint8  linkage_type, hand_over_type, origin_type;
	guint   end = offset + len;
	guint   population_id_loop_count;
	guint16 population_id_base, population_id_mask;

	proto_item *pi;
	proto_tree *population_tree;

	proto_tree_add_item(tree, hf_mpeg_descr_linkage_transport_stream_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_linkage_original_network_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_linkage_service_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_linkage_linkage_type, tvb, offset, 1, ENC_BIG_ENDIAN);
	linkage_type = tvb_get_guint8(tvb, offset);
	offset++;

	if (linkage_type == 0x08) {
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_hand_over_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_origin_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		hand_over_type = (tvb_get_guint8(tvb, offset) & MPEG_DESCR_LINKAGE_HAND_OVER_TYPE_MASK) >> MPEG_DESCR_LINKAGE_HAND_OVER_TYPE_SHIFT;
		origin_type = tvb_get_guint8(tvb, offset) & MPEG_DESCR_LINKAGE_ORIGIN_TYPE_MASK;
		offset++;

		if (hand_over_type == 1 || hand_over_type == 2 || hand_over_type == 3) {
			proto_tree_add_item(tree, hf_mpeg_descr_linkage_network_id, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;
		}

		if (origin_type) {
			proto_tree_add_item(tree, hf_mpeg_descr_linkage_initial_service_id, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;
		}

	} else if (linkage_type == 0x0D) {
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_target_event_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_linkage_target_listed, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_event_simulcast, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_reserved2, tvb, offset, 1, ENC_BIG_ENDIAN);
	} else if (linkage_type == 0x81) {
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_interactive_network_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		population_id_loop_count = tvb_get_guint8(tvb, offset) + 1;
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_population_id_loop_count, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		while (population_id_loop_count--) {

			population_id_base = tvb_get_ntohs(tvb, offset);
			population_id_mask = tvb_get_ntohs(tvb, offset + 2);
			pi = proto_tree_add_string_format_value(tree, hf_mpeg_descr_linkage_population_id, tvb, offset, 4, "Population ID", "0x%04x/0x%04x",
				population_id_base, population_id_mask);
			population_tree = proto_item_add_subtree(pi, ett_mpeg_descriptor_linkage_population_id);

			proto_tree_add_item(population_tree, hf_mpeg_descr_linkage_population_id_base, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;

			proto_tree_add_item(population_tree, hf_mpeg_descr_linkage_population_id_mask, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;
		}

	}

	if (end - offset > 0)
		proto_tree_add_item(tree, hf_mpeg_descr_linkage_private_data_byte, tvb, offset, end - offset, ENC_NA);
}
