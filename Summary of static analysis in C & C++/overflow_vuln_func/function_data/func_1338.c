static void
dissect_dscreq (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *dscreq_tree;
  guint16 tlv_data_len;
  guint16 transid;
  tvbuff_t *next_tvb;


  transid = tvb_get_ntohs (tvb, 0);
  tlv_data_len = tvb_length_remaining (tvb, 2);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Dynamic Service Change Request Tran-id = %u", transid);
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_dscreq, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"DSC Request");
      dscreq_tree = proto_item_add_subtree (it, ett_docsis_dscreq);
      proto_tree_add_item (dscreq_tree, hf_docsis_dscreq_tranid, tvb, 0, 2,
			   FALSE);

      /
      next_tvb = tvb_new_subset (tvb, 2, tlv_data_len, tlv_data_len);
      call_dissector (docsis_tlv_handle, next_tvb, pinfo, dscreq_tree);
    }

}
