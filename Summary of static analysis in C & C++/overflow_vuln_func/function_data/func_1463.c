guint
proto_mpeg_descriptor_loop_dissect(tvbuff_t *tvb, guint offset, guint loop_len, proto_tree *tree)
{
	/
	guint32  private_data_specifier = PRIVATE_DATA_SPECIFIER_RESERVED;
	guint    offset_start;
	guint    desc_len;
	guint8   tag;

	offset_start = offset;

	while (offset-offset_start < loop_len) {
		/
		tag = tvb_get_guint8(tvb, offset);
		if (tag==0x5F) {
			/
			/
			private_data_specifier = tvb_get_ntohl(tvb, offset+2);
		}

		 /
		if (match_strval(tag, mpeg_descriptor_tag_vals)) {
			desc_len = proto_mpeg_descriptor_dissect(tvb, offset, tree);
		}
		else {
			switch (private_data_specifier) {
				case PRIVATE_DATA_SPECIFIER_CIPLUS_LLP:
					desc_len = proto_mpeg_descriptor_dissect_private_ciplus(tvb, offset, tree);
					break;
				default:
					desc_len = 0;
					break;
			}
			if (desc_len==0) {
				/
				desc_len = proto_mpeg_descriptor_dissect(tvb, offset, tree);
			}
		}

		offset += desc_len;
	}

	return offset-offset_start;
}
