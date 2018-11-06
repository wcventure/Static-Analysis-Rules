static void
dissect_dscrsp (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *dscrsp_tree;
  guint16 transid;
  guint16 tlv_data_len;
  guint8 response;
  tvbuff_t *next_tvb;


  transid = tvb_get_ntohs (tvb, 0);
  response = tvb_get_guint8 (tvb, 2);
  tlv_data_len = tvb_length_remaining (tvb, 3);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Dynamic Service Change Response ID = %u (%s)", transid,
		    val_to_str (response, docsis_conf_code, "%s"));
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_dscrsp, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"DSC Response");
      dscrsp_tree = proto_item_add_subtree (it, ett_docsis_dscrsp);
      proto_tree_add_item (dscrsp_tree, hf_docsis_dscrsp_tranid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (dscrsp_tree, hf_docsis_dscrsp_response, tvb, 2, 1,
			   FALSE);

      /
      next_tvb = tvb_new_subset (tvb, 3, tlv_data_len, tlv_data_len);
      call_dissector (docsis_tlv_handle, next_tvb, pinfo, dscrsp_tree);
    }
}
