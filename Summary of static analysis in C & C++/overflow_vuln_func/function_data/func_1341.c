static void
dissect_dsdrsp (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *dsdrsp_tree;
  guint16 tranid;
  guint8 confcode;

  tranid = tvb_get_ntohs (tvb, 0);
  confcode = tvb_get_guint8 (tvb, 2);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Dynamic Service Delete Response Tran id = %u (%s)",
		    tranid, val_to_str (confcode, docsis_conf_code, "%s"));
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_dsdrsp, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"DSD Response");
      dsdrsp_tree = proto_item_add_subtree (it, ett_docsis_dsdrsp);
      proto_tree_add_item (dsdrsp_tree, hf_docsis_dsdrsp_tranid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (dsdrsp_tree, hf_docsis_dsdrsp_confcode, tvb, 2, 1,
			   FALSE);
      proto_tree_add_item (dsdrsp_tree, hf_docsis_dsdrsp_rsvd, tvb, 3, 1,
			   FALSE);
    }

}
