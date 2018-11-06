static void
dissect_bpkmreq (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *bpkmreq_tree;
  guint8 code;
  guint16 attrs_len;
  tvbuff_t *attrs_tvb;

  code = tvb_get_guint8 (tvb, 0);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO, "BPKM Request (%s)",
		    val_to_str (code, code_field_vals, "%s"));
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_bpkmreq, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"BPKM Request Message");
      bpkmreq_tree = proto_item_add_subtree (it, ett_docsis_bpkmreq);
      proto_tree_add_item (bpkmreq_tree, hf_docsis_bpkmreq_code, tvb, 0, 1,
			   FALSE);
      proto_tree_add_item (bpkmreq_tree, hf_docsis_bpkmreq_ident, tvb, 1, 1,
			   FALSE);
      proto_tree_add_item (bpkmreq_tree, hf_docsis_bpkmreq_length, tvb, 2, 2,
			   FALSE);
    }

  /
  attrs_len = tvb_length_remaining (tvb, 4);
  attrs_tvb = tvb_new_subset (tvb, 4, attrs_len, attrs_len);
  call_dissector (attrs_handle, attrs_tvb, pinfo, tree);



}
