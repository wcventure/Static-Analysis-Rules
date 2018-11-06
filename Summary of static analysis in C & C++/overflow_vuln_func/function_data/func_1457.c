static void
proto_mpeg_descriptor_dissect_content_identifier(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint  end = offset + len, crid_len;
	guint8 crid, crid_location, crid_type;

	proto_item *ci;
	proto_tree *crid_tree;

	while (offset < end) {
		crid = tvb_get_guint8(tvb, offset);
		crid_type = (crid & MPEG_DESCR_CONTENT_IDENTIFIER_CRID_TYPE_MASK) >> 2;
		crid_location = crid & MPEG_DESCR_CONTENT_IDENTIFIER_CRID_LOCATION_MASK;

		if (crid_location == 0) {
			crid_len = 2 + tvb_get_guint8(tvb, offset + 1);
		} else if (crid_location == 1) {
			crid_len = 3;
		} else {
			crid_len = 1;
		}

		ci = proto_tree_add_text(tree, tvb, offset, crid_len, "CRID type=0%02x", crid_type);
		crid_tree = proto_item_add_subtree(ci, ett_mpeg_descriptor_content_identifier_crid);

		proto_tree_add_item(crid_tree, hf_mpeg_descr_content_identifier_crid_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		proto_tree_add_item(crid_tree, hf_mpeg_descr_content_identifier_crid_location, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		if (crid_location == 0x00) {
			crid_len = tvb_get_guint8(tvb, offset);
			proto_tree_add_item(crid_tree, hf_mpeg_descr_content_identifier_crid_length, tvb, offset, 1, ENC_BIG_ENDIAN);
			offset++;

			proto_tree_add_item(crid_tree, hf_mpeg_descr_content_identifier_crid_bytes, tvb, offset, crid_len, ENC_NA);
			offset += crid_len;
		} else if (crid_location == 0x01) {
			proto_tree_add_item(crid_tree, hf_mpeg_descr_content_identifier_cird_ref, tvb, offset, 2, ENC_BIG_ENDIAN);
		}

	}


}
