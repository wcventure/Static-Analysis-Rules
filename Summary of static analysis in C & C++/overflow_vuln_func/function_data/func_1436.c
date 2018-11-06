static void
proto_mpeg_descriptor_dissect_vbi_data(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{

	guint8 svc_id, svc_len;
	guint  end = offset + len, svc_end;

	proto_item *si;
	proto_tree *svc_tree;

	while (offset < end) {
		svc_id = tvb_get_guint8(tvb, offset);
		svc_len = tvb_get_guint8(tvb, offset + 1);
		si = proto_tree_add_text(tree, tvb, offset, svc_len + 2, "Service 0x%02x", svc_id);
		svc_tree = proto_item_add_subtree(si, ett_mpeg_descriptor_vbi_data_service);

		proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_service_id, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_descr_len, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		switch (svc_id) {
			case 0x01:
			case 0x02:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
				svc_end = offset + svc_len;
				while (offset < svc_end) {
					proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_reserved1, tvb, offset, 1, ENC_BIG_ENDIAN);
					proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_field_parity, tvb, offset, 1, ENC_BIG_ENDIAN);
					proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_line_offset, tvb, offset, 1, ENC_BIG_ENDIAN);
					offset++;
				}
				break;
			default:
				proto_tree_add_item(svc_tree, hf_mpeg_descr_vbi_data_reserved2, tvb, offset, svc_len, ENC_NA);
				offset += svc_len;
				break;
		}

	}
}
