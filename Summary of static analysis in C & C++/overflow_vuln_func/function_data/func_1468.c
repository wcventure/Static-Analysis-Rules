static void
dissect_dsmcc_dii(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
		guint offset)
{
	guint8 module_info_len;
	guint16 modules, private_data_len;
	guint16 module_id;
	guint8 module_version;
	guint module_size;
	guint i;
	proto_item *pi;
	proto_tree *mod_tree;

	proto_tree_add_item(tree, hf_dsmcc_dii_download_id, tvb, offset, 4, ENC_BIG_ENDIAN);
	offset += 4;
	proto_tree_add_item(tree, hf_dsmcc_dii_block_size, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(tree, hf_dsmcc_dii_window_size, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(tree, hf_dsmcc_dii_ack_period, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(tree, hf_dsmcc_dii_t_c_download_window, tvb, offset, 4, ENC_BIG_ENDIAN);
	offset += 4;
	proto_tree_add_item(tree, hf_dsmcc_dii_t_c_download_scenario, tvb, offset, 4, ENC_BIG_ENDIAN);
	offset += 4;
	offset += dissect_dsmcc_dii_compat_desc(tvb, pinfo, tree, offset);
	proto_tree_add_item(tree, hf_dsmcc_dii_number_of_modules, tvb, offset, 2, ENC_BIG_ENDIAN);
	modules = tvb_get_ntohs(tvb, offset);
	offset += 2;

	for (i = 0; i < modules; i++ ) {
		module_id = tvb_get_ntohs(tvb, offset);
		module_size = tvb_get_ntohl(tvb, 2+offset);
		module_version = tvb_get_guint8(tvb, 6+offset);

		pi = proto_tree_add_text(tree, tvb, offset, -1,
				"Module Id: 0x%x, Version: %u, Size: %u",
				module_id, module_version, module_size);
		mod_tree = proto_item_add_subtree(pi, ett_dsmcc_dii_module);
		proto_tree_add_item(mod_tree, hf_dsmcc_dii_module_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
		proto_tree_add_item(mod_tree, hf_dsmcc_dii_module_size, tvb, offset, 4, ENC_BIG_ENDIAN);
		offset += 4;
		proto_tree_add_item(mod_tree, hf_dsmcc_dii_module_version, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		module_info_len = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(mod_tree, hf_dsmcc_dii_module_info_length, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		if (0 < module_info_len) {
			proto_tree_add_item(mod_tree, hf_etv_module_abs_path, tvb, offset, 1,
				ENC_ASCII|ENC_NA);
			offset += module_info_len;
		}
	}

	private_data_len = tvb_get_ntohs(tvb, offset);
	proto_tree_add_item(tree, hf_dsmcc_dii_private_data_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	if (0 < private_data_len) {
		proto_tree_add_item(tree, hf_etv_dii_authority, tvb, offset, 1,
			ENC_ASCII|ENC_NA);
		offset += private_data_len;
	}
}
