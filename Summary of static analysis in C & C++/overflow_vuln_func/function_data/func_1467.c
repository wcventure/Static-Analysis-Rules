static guint
dissect_dsmcc_dii_compat_desc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
				guint offset)
{
	gint i, j;
	guint8 sub_count, sub_len;
	guint16 len, count;
	proto_item *pi;
	proto_tree *compat_tree;
	proto_tree *desc_sub_tree;

	len = tvb_get_ntohs(tvb, offset);
	proto_tree_add_item(tree, hf_compat_desc_length, tvb, offset,
				2, ENC_BIG_ENDIAN);
	offset += 2;

	if (0 < len) {
		count = tvb_get_ntohs(tvb, offset);
		proto_tree_add_item(tree, hf_compat_desc_count, tvb, offset,
					2, ENC_BIG_ENDIAN);
		offset += 2;

		for (i = 0; i < count; i++) {
			pi = proto_tree_add_text(tree, tvb, offset, len, "Compatibility Descriptor");
			compat_tree = proto_item_add_subtree(pi, ett_dsmcc_compat);
			proto_tree_add_item(compat_tree, hf_desc_type, tvb, offset,
						1, ENC_BIG_ENDIAN);
			offset++;
			proto_tree_add_item(compat_tree, hf_desc_length, tvb, offset,
						1, ENC_BIG_ENDIAN);
			offset++;
			proto_tree_add_item(compat_tree, hf_desc_spec_type, tvb, offset,
						1, ENC_BIG_ENDIAN);
			offset++;
			proto_tree_add_item(compat_tree, hf_desc_spec_data, tvb, offset,
						3, ENC_BIG_ENDIAN);
			offset += 3;
			proto_tree_add_item(compat_tree, hf_desc_model, tvb, offset,
						2, ENC_BIG_ENDIAN);
			offset += 2;
			proto_tree_add_item(compat_tree, hf_desc_version, tvb, offset,
						2, ENC_BIG_ENDIAN);
			offset += 2;

			sub_count = tvb_get_guint8(tvb, offset);
			proto_tree_add_item(compat_tree, hf_desc_sub_desc_count, tvb, offset,
						1, ENC_BIG_ENDIAN);
			offset++;

			for (j = 0; j < sub_count; j++) {
				sub_len = tvb_get_guint8(tvb, offset+1);

				pi = proto_tree_add_text(compat_tree, tvb, offset, sub_len+2, "Sub Descriptor");
				desc_sub_tree = proto_item_add_subtree(pi, ett_dsmcc_compat_sub_desc);
				proto_tree_add_item(desc_sub_tree, hf_desc_sub_desc_type, tvb, offset,
							1, ENC_BIG_ENDIAN);
				offset++;
				proto_tree_add_item(desc_sub_tree, hf_desc_sub_desc_len, tvb, offset,
							1, ENC_BIG_ENDIAN);
				offset++;

				offset += sub_len;
			}
		}

		if( 1000 == offset ) {
			expert_add_info_format( pinfo, NULL, PI_MALFORMED,
						PI_ERROR, "Invalid CRC" );
		}
	}

	return 2 + len;
}
