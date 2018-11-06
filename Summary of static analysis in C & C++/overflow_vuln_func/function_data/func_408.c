static void dissect_nwmtp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	int offset = 0;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "NW MTP");
	col_clear(pinfo->cinfo, COL_INFO);

	while (tvb_reported_length_remaining(tvb, offset) > 0) {
		const gchar *type;
		proto_item *ti;
		proto_item *nwmtp_tree;
		guint32 len;
		tvbuff_t *next_tvb;

		/
		type = val_to_str_const(tvb_get_guint8(tvb, offset + 1),
					nwmtp_data_type_vals, "Unknown");
		col_set_str(pinfo->cinfo, COL_INFO, type);

		len = tvb_get_ntohl(tvb, offset + 8);

		if (tree) {
			ti = proto_tree_add_protocol_format(tree, proto_nwmtp,
					tvb, offset, len + 12,
					"NexusWare C7 UDP Protocol");

			nwmtp_tree = proto_item_add_subtree(ti, ett_mwmtp);
			proto_tree_add_item(nwmtp_tree, hf_nwmtp_transp_type,
					    tvb, offset, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(nwmtp_tree, hf_nwmtp_data_type,
					    tvb, offset + 1, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(nwmtp_tree, hf_nwmtp_data_index,
					    tvb, offset + 2, 2, ENC_BIG_ENDIAN);
			proto_tree_add_item(nwmtp_tree, hf_nwmtp_user_context,
					    tvb, offset + 4, 4, ENC_BIG_ENDIAN);
			proto_tree_add_item(nwmtp_tree, hf_nwmtp_data_length,
					    tvb, offset + 8, 4, ENC_BIG_ENDIAN);
		}

		next_tvb = tvb_new_subset(tvb, offset + 12, len, len);
		if (tvb_length(next_tvb) > 0)
			call_dissector(mtp_handle, next_tvb, pinfo, tree);
		offset += len + 12;
	}
}
