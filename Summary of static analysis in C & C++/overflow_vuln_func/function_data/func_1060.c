void
isis_dissect_isis_lsp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset,
	int lsp_type, int header_length, int id_length)
{
	proto_item	*ti, *to, *ta;
	proto_tree	*lsp_tree = NULL, *info_tree, *att_tree;
	guint16		pdu_length;
	guint8		lsp_info, lsp_att;
	int		len;

	if (tree) {
		ti = proto_tree_add_text(tree, tvb, offset, -1,
		    PROTO_STRING_LSP);
		lsp_tree = proto_item_add_subtree(ti, ett_isis_lsp);
	}

	pdu_length = tvb_get_ntohs(tvb, offset);
	if (tree) {
		proto_tree_add_uint(lsp_tree, hf_isis_lsp_pdu_length, tvb,
			offset, 2, pdu_length);
	}
	offset += 2;

	if (tree) {
		proto_tree_add_text(lsp_tree, tvb, offset, 2,
                                    "Remaining Lifetime: %us",
                                    tvb_get_ntohs(tvb, offset));
	}
	offset += 2;

	if (tree) {
		proto_tree_add_text(lsp_tree, tvb, offset, id_length + 2,
                                    "LSP-ID: %s",
                                    print_system_id( tvb_get_ptr(tvb, offset, id_length+2), id_length+2 ) );                
	}

	if (check_col(pinfo->cinfo, COL_INFO)) {
	    col_append_fstr(pinfo->cinfo, COL_INFO, ", LSP-ID: %s",
			print_system_id( tvb_get_ptr(tvb, offset, id_length+2), id_length+2 ) );
	}
	offset += id_length + 2;

	if (tree) {
		proto_tree_add_uint(lsp_tree, hf_isis_lsp_sequence_number, tvb,
			offset, 4,
			tvb_get_ntohl(tvb, offset));
	}
	if (check_col(pinfo->cinfo, COL_INFO)) {
		col_append_fstr(pinfo->cinfo, COL_INFO, ", Sequence: 0x%08x, Lifetime: %5us",
			tvb_get_ntohl(tvb, offset),
			tvb_get_ntohs(tvb, offset - (id_length+2+2)));
	}
	offset += 4;

	if (tree) {
		/
		proto_tree_add_uint(lsp_tree, hf_isis_lsp_checksum, tvb,
			offset, 2, tvb_get_ntohs(tvb, offset));
	}
	offset += 2;

	if (tree) {
		/
		lsp_info = tvb_get_guint8(tvb, offset);
		to = proto_tree_add_text(lsp_tree, tvb, offset, 1,
			"Type block(0x%02x): Partition Repair:%d, Attached bits:%d, Overload bit:%d, IS type:%d",
			lsp_info,
			ISIS_LSP_PARTITION(lsp_info),
			ISIS_LSP_ATT(lsp_info),
			ISIS_LSP_HIPPITY(lsp_info),
			ISIS_LSP_IS_TYPE(lsp_info)
			);

		info_tree = proto_item_add_subtree(to, ett_isis_lsp_info);
		proto_tree_add_boolean(info_tree, hf_isis_lsp_p, tvb, offset, 1, lsp_info);
		ta = proto_tree_add_uint(info_tree, hf_isis_lsp_att, tvb, offset, 1, lsp_info);
		att_tree = proto_item_add_subtree(ta, ett_isis_lsp_att);
		lsp_att = ISIS_LSP_ATT(lsp_info);
		proto_tree_add_text(att_tree, tvb, offset, 1,
			  "%d... = Default metric: %s", ISIS_LSP_ATT_DEFAULT(lsp_att), ISIS_LSP_ATT_DEFAULT(lsp_att) ? "Set" : "Unset");
		proto_tree_add_text(att_tree, tvb, offset, 1,
			  ".%d.. = Delay metric: %s", ISIS_LSP_ATT_DELAY(lsp_att), ISIS_LSP_ATT_DELAY(lsp_att) ? "Set" : "Unset");
		proto_tree_add_text(att_tree, tvb, offset, 1,
			  "..%d. = Expense metric: %s", ISIS_LSP_ATT_EXPENSE(lsp_att), ISIS_LSP_ATT_EXPENSE(lsp_att) ? "Set" : "Unset");
		proto_tree_add_text(att_tree, tvb, offset, 1,
			  "...%d = Error metric: %s", ISIS_LSP_ATT_ERROR(lsp_att), ISIS_LSP_ATT_ERROR(lsp_att) ? "Set" : "Unset");
		proto_tree_add_boolean(info_tree, hf_isis_lsp_hippity, tvb, offset, 1, lsp_info);
		proto_tree_add_uint(info_tree, hf_isis_lsp_is_type, tvb, offset, 1, lsp_info);
	}
	offset += 1;

	len = pdu_length - header_length;
	if (len < 0) {
		isis_dissect_unknown(tvb, tree, offset,
			"packet header length %d went beyond packet",
			 header_length );
		return;
	}
	/
	if (lsp_type == ISIS_TYPE_L1_LSP){
		isis_dissect_clvs(tvb, lsp_tree, offset,
			clv_l1_lsp_opts, len, id_length,
			ett_isis_lsp_clv_unknown );
	} else {
		isis_dissect_clvs(tvb, lsp_tree, offset,
			clv_l2_lsp_opts, len, id_length,
			ett_isis_lsp_clv_unknown );
	}
}
