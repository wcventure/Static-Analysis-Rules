static void
dissect_regack (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *regack_tree;
  guint16 sid;
  guint16 tlv_data_len;
  guint8 response;
  tvbuff_t *next_tvb;

  sid = tvb_get_ntohs (tvb, 0);
  response = tvb_get_guint8 (tvb, 2);
  tlv_data_len = tvb_length_remaining (tvb, 3);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Registration Acknowledge SID = %u (%s)", sid,
		    val_to_str (response, docsis_conf_code, "%s"));
    }
  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_regack, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"Registration Acknowledge");
      regack_tree = proto_item_add_subtree (it, ett_docsis_regack);
      proto_tree_add_item (regack_tree, hf_docsis_regack_sid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (regack_tree, hf_docsis_regack_response, tvb, 2, 1,
			   FALSE);

      /
      next_tvb = tvb_new_subset (tvb, 3, tlv_data_len, tlv_data_len);
      call_dissector (docsis_tlv_handle, next_tvb, pinfo, regack_tree);
    }

}
