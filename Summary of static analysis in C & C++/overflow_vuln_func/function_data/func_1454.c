static void
proto_mpeg_descriptor_dissect_ac3(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint  end = offset + len;
	guint8 flags, component_type;

	proto_item *ci;
	proto_tree *component_type_tree;

	flags = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_mpeg_descr_ac3_component_type_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_ac3_bsid_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_ac3_mainid_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_ac3_asvc_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_ac3_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	if (flags & MPEG_DESCR_AC3_COMPONENT_TYPE_FLAG_MASK) {
		component_type = tvb_get_guint8(tvb, offset);
		ci = proto_tree_add_text(tree, tvb, offset, 3, "Component Type 0x%02x", component_type);
		component_type_tree = proto_item_add_subtree(ci, ett_mpeg_descriptor_ac3_component_type);
		proto_tree_add_item(component_type_tree, hf_mpeg_descr_ac3_component_type_reserved_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(component_type_tree, hf_mpeg_descr_ac3_component_type_full_service_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(component_type_tree, hf_mpeg_descr_ac3_component_type_service_type_flags, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(component_type_tree, hf_mpeg_descr_ac3_component_type_number_of_channels_flags, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}

	if (flags & MPEG_DESCR_AC3_BSID_FLAG_MASK) {
		proto_tree_add_item(tree, hf_mpeg_descr_ac3_bsid, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}

	if (flags & MPEG_DESCR_AC3_MAINID_FLAG_MASK) {
		proto_tree_add_item(tree, hf_mpeg_descr_ac3_mainid, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}

	if (flags & MPEG_DESCR_AC3_ASVC_FLAG_MASK) {
		proto_tree_add_item(tree, hf_mpeg_descr_ac3_asvc, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}

	if (offset < end)
		proto_tree_add_item(tree, hf_mpeg_descr_ac3_additional_info, tvb, offset, end - offset, ENC_NA);
}
