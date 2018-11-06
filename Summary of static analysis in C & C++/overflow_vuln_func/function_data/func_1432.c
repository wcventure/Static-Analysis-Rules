static void
proto_mpeg_descriptor_dissect_service_list(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{
	guint   end = offset + len;
	guint16 svc_id;

	proto_item *si;
	proto_tree *svc_tree;


	while (offset < end) {
		svc_id = tvb_get_ntohs(tvb, offset);

		si = proto_tree_add_text(tree, tvb, offset, 3, "Service 0x%02x", svc_id);
		svc_tree = proto_item_add_subtree(si, ett_mpeg_descriptor_service_list);

		proto_tree_add_item(svc_tree, hf_mpeg_descr_service_list_id, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(svc_tree, hf_mpeg_descr_service_list_type, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;
	}
}
