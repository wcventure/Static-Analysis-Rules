static void
dissect_lsp_ext_ip_reachability_clv(tvbuff_t *tvb, proto_tree *tree,
	int offset, int id_length _U_, int length)
{
	proto_item *pi = NULL;
	proto_tree *subtree = NULL;
	proto_tree *subtree2 = NULL;
	guint8     ctrl_info;
	guint8     bit_length, byte_length;
	guint8     prefix [4];
	guint32    metric;
	guint8     len,i;
	guint8     subclvs_len;
	guint8     clv_code, clv_len;

	if (!tree) return;

	while (length > 0) {
		memset (prefix, 0, 4);
		ctrl_info = tvb_get_guint8(tvb, offset+4);
		bit_length = ctrl_info & 0x3f;
		byte_length = (bit_length + 7) / 8;
		tvb_memcpy (tvb, prefix, offset+5, byte_length);
		metric = tvb_get_ntohl(tvb, offset);
		subclvs_len = 0;
		if ((ctrl_info & 0x40) != 0)
                        subclvs_len = 1+tvb_get_guint8(tvb, offset+5+byte_length);

		pi = proto_tree_add_text (tree, tvb, offset, 5+byte_length+subclvs_len,
                                          "IPv4 prefix: %s/%d, Metric: %u, Distribution: %s, %ssub-TLVs present",
                                          ip_to_str (prefix),
                                          bit_length,
                                          metric,
                                          ((ctrl_info & 0x80) == 0) ? "up" : "down",
                                          ((ctrl_info & 0x40) == 0) ? "no " : "" );

                /
		subtree = proto_item_add_subtree (pi, ett_isis_lsp_part_of_clv_ext_ip_reachability);

		proto_tree_add_text (subtree, tvb, offset+5, byte_length, "IPv4 prefix: %s/%u",
                                     ip_to_str (prefix),
                                     bit_length);

		proto_tree_add_text (subtree, tvb, offset, 4, "Metric: %u", metric);

		proto_tree_add_text (subtree, tvb, offset+4, 1, "Distribution: %s",
                                     ((ctrl_info & 0x80) == 0) ? "up" : "down");

		len = 5 + byte_length;
		if ((ctrl_info & 0x40) != 0) {
                        subclvs_len = tvb_get_guint8(tvb, offset+len);
                        pi = proto_tree_add_text (subtree, tvb, offset+len, 1, "sub-TLVs present, total length: %u bytes",
                                             subclvs_len);
                        proto_item_set_len (pi, subclvs_len+1);
                        /
                        subtree2 = proto_item_add_subtree (pi, ett_isis_lsp_clv_ip_reach_subclv);

                        i =0;
                        while (i < subclvs_len) {
				clv_code = tvb_get_guint8(tvb, offset+len+1); /
				clv_len  = tvb_get_guint8(tvb, offset+len+2);
                                
                                /
                                dissect_ipreach_subclv(tvb, subtree2, offset+len+3, clv_code, clv_len);
                                i += clv_len + 2;
                        }
                        len += 1 + subclvs_len;
                } else {
                        proto_tree_add_text (subtree, tvb, offset+4, 1, "no sub-TLVs present");
                        proto_item_set_len (pi, len);
                }

		offset += len;
		length -= len;
	}
}
