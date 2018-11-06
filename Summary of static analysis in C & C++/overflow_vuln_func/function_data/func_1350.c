static void
dissect_uccrsp (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *uccrsp_tree;
  guint8 chid;

  chid = tvb_get_guint8 (tvb, 0);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      col_add_fstr (pinfo->cinfo, COL_INFO,
		    "Upstream Channel Change response  Channel ID = %u (U%u)",
		    chid, (chid > 0 ? chid - 1 : chid));
    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_uccrsp, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"UCC Response");
      uccrsp_tree = proto_item_add_subtree (it, ett_docsis_uccrsp);
      proto_tree_add_item (uccrsp_tree, hf_docsis_uccrsp_upchid, tvb, 0, 1,
			   FALSE);
    }

}
