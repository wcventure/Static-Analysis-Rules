static void
dissect_bicc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

/
	proto_item *ti;
	proto_tree *bicc_tree = NULL;
	tvbuff_t *message_tvb;
	guint32 bicc_cic;
	guint8 message_type;

	/

/
	if (check_col(pinfo->cinfo, COL_PROTOCOL))
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "BICC");

/
	message_type = tvb_get_guint8(tvb, BICC_CIC_OFFSET + BICC_CIC_LENGTH);

	if (check_col(pinfo->cinfo, COL_INFO))
		col_add_fstr(pinfo->cinfo, COL_INFO, "%s ", val_to_str(message_type, isup_message_type_value_acro, "reserved"));

	/
	bicc_cic = tvb_get_letohl(tvb, BICC_CIC_OFFSET);

	pinfo->ctype = CT_BICC;
	pinfo->circuit_id = bicc_cic;

	/
	if (tree) {
		ti = proto_tree_add_item(tree, proto_bicc, tvb, 0, -1, FALSE);
		bicc_tree = proto_item_add_subtree(ti, ett_bicc);


		proto_tree_add_uint_format(bicc_tree, hf_bicc_cic, tvb, BICC_CIC_OFFSET, BICC_CIC_LENGTH, bicc_cic, "CIC: %u", bicc_cic);
	}
	
	message_tvb = tvb_new_subset(tvb, BICC_CIC_LENGTH, -1, -1);
	dissect_isup_message(message_tvb, pinfo, bicc_tree);
}
