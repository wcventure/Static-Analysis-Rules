static void
dissect_dsdreq (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *dsdreq_tree;
  guint16 tlv_data_len;
  guint16 transid;
  tvbuff_t *next_tvb;

  transid = tvb_get_ntohs (tvb, 0);
  tlv_data_len = tvb_length_remaining (tvb, 8);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Dynamic Service Delete Request Tran-id = %u", transid);
    }
  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_dsdreq, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"DSD Request");
      dsdreq_tree = proto_item_add_subtree (it, ett_docsis_dsdreq);
      proto_tree_add_item (dsdreq_tree, hf_docsis_dsdreq_tranid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (dsdreq_tree, hf_docsis_dsdreq_rsvd, tvb, 2, 2,
			   FALSE);
      proto_tree_add_item (dsdreq_tree, hf_docsis_dsdreq_sfid, tvb, 4, 4,
			   FALSE);

      /
      next_tvb = tvb_new_subset (tvb, 8, tlv_data_len, tlv_data_len);
      call_dissector (docsis_tlv_handle, next_tvb, pinfo, dsdreq_tree);
    }
}
