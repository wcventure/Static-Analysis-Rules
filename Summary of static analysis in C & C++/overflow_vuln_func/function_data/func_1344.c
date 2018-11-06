static void
dissect_regreq (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *regreq_tree;
  guint16 sid;
  guint16 tlv_data_len;
  tvbuff_t *next_tvb;

  sid = tvb_get_ntohs (tvb, 0);
  tlv_data_len = tvb_length_remaining (tvb, 2);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO, "Registration Request SID = %u",
		    sid);
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_regreq, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"Registration Request");
      regreq_tree = proto_item_add_subtree (it, ett_docsis_regreq);
      proto_tree_add_item (regreq_tree, hf_docsis_regreq_sid, tvb, 0, 2,
			   FALSE);
      /
      next_tvb = tvb_new_subset (tvb, 2, tlv_data_len, tlv_data_len);
      call_dissector (docsis_tlv_handle, next_tvb, pinfo, regreq_tree);
    }



}
