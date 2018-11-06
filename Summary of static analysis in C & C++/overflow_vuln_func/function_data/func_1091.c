static void get_BCR(tvbuff_t *tvb, guint8 *offset, proto_tree *iec104_header_tree)
{
	proto_tree_add_item(iec104_header_tree, hf_bcr_count, tvb, *offset, 4, ENC_LITTLE_ENDIAN);
	*offset += 4;
	proto_tree_add_item(iec104_header_tree, hf_bcr_sq, tvb, *offset, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(iec104_header_tree, hf_bcr_cy, tvb, *offset, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(iec104_header_tree, hf_bcr_ca, tvb, *offset, 1, ENC_LITTLE_ENDIAN);
	proto_tree_add_item(iec104_header_tree, hf_bcr_iv, tvb, *offset, 1, ENC_LITTLE_ENDIAN);
	*offset += 1;
}
