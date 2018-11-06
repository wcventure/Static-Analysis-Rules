static guint
dissect_etv_bif_platform_ids(tvbuff_t *tvb, proto_tree *tree, guint offset)
{
	proto_tree *platform_tree = NULL;
	proto_item *pi;

	pi = proto_tree_add_text(tree, tvb, offset, 15, "Platform Id");
	platform_tree = proto_item_add_subtree(pi, ett_eiss_platform_id);
	proto_tree_add_item(platform_tree, hf_pdtHWManufacturer, tvb, offset, 3, ENC_BIG_ENDIAN);
	offset += 3;
	proto_tree_add_item(platform_tree, hf_pdtHWModel, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(platform_tree, hf_pdtHWVersionMajor, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(platform_tree, hf_pdtHWVersionMinor, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(platform_tree, hf_pdtSWManufacturer, tvb, offset, 3, ENC_BIG_ENDIAN);
	offset += 3;
	proto_tree_add_item(platform_tree, hf_pdtSWModel, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(platform_tree, hf_pdtSWVersionMajor, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(platform_tree, hf_pdtSWVersionMinor, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(platform_tree, hf_pdtProfile, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	return 15;
}
