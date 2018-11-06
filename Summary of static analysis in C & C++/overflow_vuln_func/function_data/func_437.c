static void
dissect_lsp_ext_is_reachability_clv(tvbuff_t *tvb, proto_tree *tree,
	int offset, int id_length _U_, int length)
{
	proto_item *ti;
	proto_tree *ntree = NULL;
	guint8     subclvs_len;
	guint8     len, i;
	guint8     clv_code, clv_len;

	if (!tree) return;

	while (length > 0) {
		ti = proto_tree_add_text (tree, tvb, offset, -1,
			"IS neighbor: %s",
			print_system_id (tvb_get_ptr(tvb, offset, 7), 7) );
		ntree = proto_item_add_subtree (ti,
			ett_isis_lsp_part_of_clv_ext_is_reachability );

		proto_tree_add_text (ntree, tvb, offset+7, 3,
			"Metric: %d", tvb_get_ntoh24(tvb, offset+7) );

		subclvs_len = tvb_get_guint8(tvb, offset+10);
		if (subclvs_len == 0) {
			proto_tree_add_text (ntree, tvb, offset+10, 1, "no sub-TLVs present");
		}
		else {
			i = 0;
			while (i < subclvs_len) {
				clv_code = tvb_get_guint8(tvb, offset+11+i);
				clv_len  = tvb_get_guint8(tvb, offset+12+i);
				switch (clv_code) {
				case 3 :
					dissect_subclv_admin_group(tvb, ntree, offset+13+i);
					break;
				case 6 :
					proto_tree_add_text (ntree, tvb, offset+11+i, 6,
						"IPv4 interface address: %s", ip_to_str (tvb_get_ptr(tvb, offset+13+i, 4)) );
					break;
				case 8 :
					proto_tree_add_text (ntree, tvb, offset+11+i, 6,
						"IPv4 neighbor address: %s", ip_to_str (tvb_get_ptr(tvb, offset+13+i, 4)) );
					break;
				case 9 :
					dissect_subclv_max_bw (tvb, ntree, offset+13+i);
					break;
				case 10:
					dissect_subclv_rsv_bw (tvb, ntree, offset+13+i);
					break;
				case 11:
					dissect_subclv_unrsv_bw (tvb, ntree, offset+13+i);
					break;
				case 18:
					proto_tree_add_text (ntree, tvb, offset+11+i, 5,
						"Traffic engineering default metric: %d",
						tvb_get_ntoh24(tvb, offset+13+i) );
					break;
				case 250:
				case 251:
				case 252:
				case 253:
				case 254:
					proto_tree_add_text (ntree, tvb, offset+11+i, clv_len+2,
						"Unknown Cisco specific extensions: code %d, length %d",
						clv_code, clv_len );
					break;
				default :
					proto_tree_add_text (ntree, tvb, offset+11+i, clv_len+2,
						"Unknown sub-CLV: code %d, length %d", clv_code, clv_len );
					break;
				}
				i += clv_len + 2;
			}
		}

		len = 11 + subclvs_len;
		proto_item_set_len (ti, len);
		offset += len;
		length -= len;
	}
}
