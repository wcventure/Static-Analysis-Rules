static void
dissect_lsp_ipv6_reachability_clv(tvbuff_t *tvb, proto_tree *tree, int offset,
	int id_length _U_, int length)
{
	proto_item        *pi;
	proto_tree        *subtree = NULL;
	proto_tree        *subtree2 = NULL;
	guint8            ctrl_info;
	guint8            bit_length, byte_length;
	struct e_in6_addr prefix;
	guint32           metric;
	guint8            len,i;
	guint8            subclvs_len;
	guint8            clv_code, clv_len;

	if (!tree) return;

	memset (prefix.s6_addr, 0, 16);

	while (length > 0) {
		ctrl_info = tvb_get_guint8(tvb, offset+4);
		bit_length = tvb_get_guint8(tvb, offset+5);
		byte_length = (bit_length + 7) / 8;
		tvb_memcpy (tvb, prefix.s6_addr, offset+6, byte_length);
		metric = tvb_get_ntohl(tvb, offset);
		subclvs_len = 0;
		if ((ctrl_info & 0x20) != 0)
                        subclvs_len = 1+tvb_get_guint8(tvb, offset+6+byte_length);

		pi = proto_tree_add_text (tree, tvb, offset, 6+byte_length+subclvs_len,
                                          "IPv6 prefix: %s/%u, Metric: %u, Distribution: %s, %s, %ssub-TLVs present",
                                          ip6_to_str (&prefix),
                                          bit_length,
                                          metric,
                                          ((ctrl_info & 0x80) == 0) ? "up" : "down",
                                          ((ctrl_info & 0x40) == 0) ? "internal" : "external",
                                          ((ctrl_info & 0x20) == 0) ? "no " : "" );

		subtree = proto_item_add_subtree (pi, ett_isis_lsp_part_of_clv_ipv6_reachability);

		proto_tree_add_text (subtree, tvb, offset+6, byte_length, "IPv6 prefix: %s/%u",
                                     ip6_to_str (&prefix),
                                     bit_length);

		proto_tree_add_text (subtree, tvb, offset, 4,
			"Metric: %u", metric);

		proto_tree_add_text (subtree, tvb, offset+4, 1,
			"Distribution: %s, %s",
			((ctrl_info & 0x80) == 0) ? "up" : "down",
			((ctrl_info & 0x40) == 0) ? "internal" : "external" );

		if ((ctrl_info & 0x1f) != 0) {
			proto_tree_add_text (subtree, tvb, offset+4, 1,
					     "Reserved bits: 0x%x",
					     (ctrl_info & 0x1f) );
                }

		len = 6 + byte_length;
		if ((ctrl_info & 0x20) != 0) {
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
