static void
dissect_dsmcc_ddb(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
			proto_tree *top_tree, guint offset)
{
	tvbuff_t *sub_tvb;
	proto_item *pi;
	guint8 reserved;

	proto_tree_add_item(tree, hf_dsmcc_ddb_module_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(tree, hf_dsmcc_ddb_version, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	reserved = tvb_get_guint8(tvb, offset);
	pi = proto_tree_add_item(tree, hf_dsmcc_ddb_reserved, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	if (0xff != reserved) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid value - should be 0xff");
	}
	offset++;
	proto_tree_add_item(tree, hf_dsmcc_ddb_block_number, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	sub_tvb = tvb_new_subset_remaining(tvb, offset);
	call_dissector(data_handle, sub_tvb, pinfo, top_tree);
}
