static guint
dissect_eiss_descriptors(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
				guint offset)
{
	proto_item *pi;
	proto_tree *sub_tree;
	guint tag;

	tag = tvb_get_guint8(tvb, offset);

	if (0xe0 == tag) {
		guint8 total_length;

		total_length = tvb_get_guint8(tvb, offset+1);
		pi = proto_tree_add_text(tree, tvb, offset, (2+total_length),
					"ETV Application Information Descriptor");
		sub_tree = proto_item_add_subtree(pi, ett_eiss_desc);
		proto_tree_add_item(sub_tree, hf_eiss_descriptor_tag,
					tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_descriptor_length, tvb,
					offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_app_control_code, tvb,
					offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_app_version_major, tvb,
					offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_app_version_minor, tvb,
					offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_max_proto_version_major,
					tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_max_proto_version_minor,
					tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_test_flag, tvb, offset,
					1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_aid_reserved, tvb, offset,
					3, ENC_BIG_ENDIAN);
		offset += 3;
		proto_tree_add_item(sub_tree, hf_eiss_aid_priority, tvb, offset,
					1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_irl_type, tvb, offset, 2,
					ENC_BIG_ENDIAN);
		proto_tree_add_item(sub_tree, hf_eiss_irl_length, tvb, offset,
					2, ENC_BIG_ENDIAN);
		offset += 2;
		proto_tree_add_item(sub_tree, hf_eiss_irl_string, tvb, offset, 2,
					ENC_ASCII|ENC_NA);
		return (2+total_length);
	} else if (0xe1 == tag) {
		pi = proto_tree_add_text(tree, tvb, offset, 6,
					"ETV Media Time Descriptor");
		sub_tree = proto_item_add_subtree(pi, ett_eiss_desc);
		proto_tree_add_item(sub_tree, hf_eiss_descriptor_tag,
					tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_descriptor_length, tvb,
					offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_mtd_time_value, tvb,
					offset, 4, ENC_BIG_ENDIAN);
		return 6;
	} else if (0xe2 == tag) {
		guint16 tmp;
		tvbuff_t *payload = NULL;

		tmp = tvb_get_ntohs(tvb, offset+1);
		pi = proto_tree_add_text(tree, tvb, offset, (3+tmp),
					"ETV Stream Event Descriptor");
		sub_tree = proto_item_add_subtree(pi, ett_eiss_desc);
		proto_tree_add_item(sub_tree, hf_eiss_descriptor_tag,
					tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
		proto_tree_add_item(sub_tree, hf_eiss_sed_reserved, tvb,
					offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(sub_tree, hf_eiss_sed_descriptor_length, tvb,
					offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
		proto_tree_add_item(sub_tree, hf_eiss_sed_time_value, tvb,
					offset, 4, ENC_BIG_ENDIAN);
		offset += 4;

		payload = tvb_new_subset(tvb, offset, tmp-4, tmp-4);
		call_dissector(data_handle, payload, pinfo, sub_tree);

		return (3+tmp);
	} else {
		pi = proto_tree_add_text(tree, tvb, offset, -1,
					"Unknown Descriptor");

		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Unknown Descriptor");

		/
		return 1000;
	}
}
