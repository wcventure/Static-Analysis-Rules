static void
dissect_rngreq (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  proto_item *it;
  proto_tree *rngreq_tree;
  guint16 sid;

  sid = tvb_get_ntohs (tvb, 0);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      if (sid > 0)
	col_add_fstr (pinfo->cinfo, COL_INFO, "Ranging Request: SID = %u",
		      sid);
      else
	col_add_str (pinfo->cinfo, COL_INFO,
		     "Initial Ranging Request SID = 0");
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_rngreq, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"Ranging Request");
      rngreq_tree = proto_item_add_subtree (it, ett_docsis_rngreq);
      proto_tree_add_item (rngreq_tree, hf_docsis_rngreq_sid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (rngreq_tree, hf_docsis_rngreq_down_chid, tvb, 2, 1,
			   FALSE);
      proto_tree_add_item (rngreq_tree, hf_docsis_rngreq_pend_compl, tvb, 3,
			   1, FALSE);
    }


}
