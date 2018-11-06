static int
decode_gtp_rai(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	proto_tree	*ext_tree_rai;
	proto_item	*te;
	guint8		byte[3];
	guint16		mnc, mcc;

	te = proto_tree_add_text(tree, tvb, offset, 1, val_to_str(GTP_EXT_RAI, gtp_val, "Unknown message"));
	ext_tree_rai = proto_item_add_subtree(te, ett_gtp_rai);

	byte[1] = tvb_get_guint8 (tvb, offset + 1);
	byte[2] = tvb_get_guint8 (tvb, offset + 2);
	byte[3] = tvb_get_guint8 (tvb, offset + 3);
	mcc = (byte[1] & 0x0F) * 100 + ((byte[1] & 0xF0) >> 4) * 10  + (byte[2] & 0x0F );
	if ((byte[2] & 0xF0) == 0xF0)
		mnc = (byte[3] & 0x0F) * 10  + ((byte[3] & 0xF0) >> 4);
	else
		mnc = (byte[3] & 0x0F) * 100 + ((byte[3] & 0xF0) >> 4 ) * 10  + ((byte[2] & 0xF0) >> 4);

	proto_tree_add_uint(ext_tree_rai, hf_gtp_rai_mcc, tvb, offset+1, 2, mcc);
	proto_tree_add_uint(ext_tree_rai, hf_gtp_rai_mnc, tvb, offset+2, 2, mnc);
	proto_tree_add_uint(ext_tree_rai, hf_gtp_rai_lac, tvb, offset+4, 2, tvb_get_ntohs (tvb, offset+4));
	proto_tree_add_uint(ext_tree_rai, hf_gtp_rai_rac, tvb, offset+6, 1, tvb_get_guint8 (tvb, offset+6));

	return 7;
}
