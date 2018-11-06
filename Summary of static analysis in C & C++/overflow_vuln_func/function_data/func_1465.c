static void
dissect_dsmcc_adaptation_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	tvbuff_t *sub_tvb;
	guint offset = 0;
	proto_item *pi;
	proto_tree *sub_tree;
	guint8 type, tmp;
	guint16 ca_len;

	type = tvb_get_guint8(tvb, offset);

	if (1 == type) {
		pi = proto_tree_add_text(tree, tvb, offset, -1, "Adaptation Header");
		sub_tree = proto_item_add_subtree(pi, ett_dsmcc_adaptation_header);
		proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_type, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		offset++;
		tmp = tvb_get_guint8(tvb, offset);
		pi = proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_ca_reserved, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		if (0xff != tmp) {
			PROTO_ITEM_SET_GENERATED(pi);
			expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
						"Invalid value - should be 0xff");
		}
		offset++;
		proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_ca_system_id, tvb,
			offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
		ca_len = tvb_get_ntohs(tvb, offset);
		proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_ca_length, tvb,
			offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
		sub_tvb = tvb_new_subset(tvb, offset, ca_len, ca_len);
		call_dissector(data_handle, sub_tvb, pinfo, tree);
	} else if (2 == type) {
		pi = proto_tree_add_text(tree, tvb, offset, -1, "Adaptation Header");
		sub_tree = proto_item_add_subtree(pi, ett_dsmcc_adaptation_header);
		proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_type, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		offset++;
		tmp = tvb_get_guint8(tvb, offset);
		pi = proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_user_id_reserved, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		if (0xff != tmp) {
			PROTO_ITEM_SET_GENERATED(pi);
			expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
						"Invalid value - should be 0xff");
		}
		offset++;
		/
	} else {
		pi = proto_tree_add_text(tree, tvb, offset, -1, "Unknown Adaptation Header");
		sub_tree = proto_item_add_subtree(pi, ett_dsmcc_adaptation_header);
		proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_type, tvb,
			offset, 1, ENC_BIG_ENDIAN);
	}
}
