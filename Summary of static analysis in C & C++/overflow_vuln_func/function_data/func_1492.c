static int
decode_gtp_pkt_flow_id(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	proto_tree	*ext_tree_pkt_flow_id;
	proto_item	*te;
	guint8 		nsapi, pkt_flow_id;

	nsapi = tvb_get_guint8(tvb, offset+1) & 0x0F;
	pkt_flow_id = tvb_get_guint8(tvb, offset+2);

	te = proto_tree_add_uint_format (tree, hf_gtp_pkt_flow_id, tvb, offset, 
		3, pkt_flow_id, "Packet Flow ID for NSAPI(%u) : %u", nsapi, 
		pkt_flow_id);
	ext_tree_pkt_flow_id = proto_item_add_subtree(tree, ett_gtp_pkt_flow_id);

	proto_tree_add_uint(ext_tree_pkt_flow_id, hf_gtp_nsapi, tvb, offset+1, 1, nsapi);
	proto_tree_add_uint_format(ext_tree_pkt_flow_id, hf_gtp_pkt_flow_id, tvb, 
		offset+2, 1, pkt_flow_id, "%s : %u", 
		val_to_str(GTP_EXT_PKT_FLOW_ID, gtp_val, "Unknown message"), 
		pkt_flow_id);

	return 3;
}
