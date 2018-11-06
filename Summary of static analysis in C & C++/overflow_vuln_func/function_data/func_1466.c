static guint
dissect_dsmcc_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset,
			gboolean download_header)
{
	tvbuff_t *sub_tvb;
	proto_item *pi;
	proto_tree *sub_tree;
	guint8 prot_disc;
	guint reserved;
	guint8 adaptation_len;
	guint len = 0;
	int msg_id, tx_id;

	prot_disc = tvb_get_guint8(tvb, offset);
	reserved = tvb_get_guint8(tvb, 8+offset);
	adaptation_len = tvb_get_guint8(tvb, 9+offset);

	pi = proto_tree_add_text(tree, tvb, offset, 12+adaptation_len, "DSM-CC Header");
	sub_tree = proto_item_add_subtree(pi, ett_dsmcc_header);
	pi = proto_tree_add_item(sub_tree, hf_dsmcc_protocol_discriminator, tvb,
				 offset, 1, ENC_BIG_ENDIAN);
	if (0x11 != prot_disc) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid value - should be 0x11");
	}
	offset++;
	proto_tree_add_item(sub_tree, hf_dsmcc_type, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;
	if (TRUE == download_header) {
		msg_id = hf_dsmcc_dd_message_id;
		tx_id = hf_dsmcc_dd_download_id;
	} else {
		msg_id = hf_dsmcc_message_id;
		tx_id = hf_dsmcc_transaction_id;
	}
	proto_tree_add_item(sub_tree, msg_id, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(sub_tree, tx_id, tvb,
		offset, 4, ENC_BIG_ENDIAN);
	offset += 4;
	pi = proto_tree_add_item(sub_tree, hf_dsmcc_header_reserved, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	if (0xff != reserved) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid value - should be 0xff");
	}
	offset++;

	proto_tree_add_item(sub_tree, hf_dsmcc_adaptation_length, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(sub_tree, hf_dsmcc_message_length, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	len = 12;
	if (0 < adaptation_len) {
		sub_tvb = tvb_new_subset(tvb, offset, adaptation_len, adaptation_len);
		dissect_dsmcc_adaptation_header(sub_tvb, pinfo, sub_tree);
		offset += adaptation_len;
	}

	return len;
}
