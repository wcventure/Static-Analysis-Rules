static void
dissect_docsis (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  guint8 fc;
  guint8 fctype;
  guint8 fcparm;
  guint8 ehdron;
  guint8 mac_parm;
  guint8 hdrlen;
  guint16 len_sid;
  tvbuff_t *next_tvb, *mgt_tvb;
  gint pdulen, captured_length;
  guint16 framelen;
  gboolean isfrag = FALSE;

/
  proto_item *ti;
  proto_tree *docsis_tree;
/
  static guint16 concatlen;
  static guint16 concatpos;

/
  fc = tvb_get_guint8 (tvb, 0);	/
  fctype = (fc >> 6) & 0x03;	/
  fcparm = (fc >> 1) & 0x1F;	/
  ehdron = (fc & 0x01);		/

  mac_parm = tvb_get_guint8 (tvb, 1);	/
  len_sid = tvb_get_ntohs (tvb, 2);	/

/
  if (ehdron == 0x00)
    hdrlen = 6;
  else
    hdrlen = 6 + mac_parm;

/
  captured_length = tvb_length_remaining (tvb, hdrlen);

/
  if ((fctype == FCTYPE_MACSPC) && fcparm == 0x02)
    {
      pdulen = 0;
      framelen = 6;
    }
  else
    {
      framelen = 6 + len_sid;
      pdulen = len_sid - (mac_parm + 2);
    }

/
  if ((fctype == FCTYPE_MACSPC) && (fcparm == 0x1c))
    {
      concatlen = len_sid;
      concatpos = 6;
    }

/
  if (check_col (pinfo->cinfo, COL_PROTOCOL))
    col_set_str (pinfo->cinfo, COL_PROTOCOL, "DOCSIS");

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      switch (fctype)
	{
	case FCTYPE_PACKET:
	  col_set_str (pinfo->cinfo, COL_INFO, "Packet PDU");
	  break;
	case FCTYPE_ATMPDU:
	  col_set_str (pinfo->cinfo, COL_INFO, "ATM PDU");
	  break;
	case FCTYPE_RESRVD:
	  col_set_str (pinfo->cinfo, COL_INFO, "Reserved PDU");
	  break;
	case FCTYPE_MACSPC:
	  if (fcparm == 0x02)
	    col_add_fstr (pinfo->cinfo, COL_INFO,
			  "Request Frame SID = %u Mini Slots = %u", len_sid,
			  mac_parm);
	  else if (fcparm == 0x03)
	    col_set_str (pinfo->cinfo, COL_INFO, "Fragmented Frame");
	  else
	    col_set_str (pinfo->cinfo, COL_INFO, "Mac Specific");
	  break;
	}			/
    }				/

/
  if (tree)
    {
      ti = proto_tree_add_protocol_format (tree, proto_docsis, tvb, 0,
					   hdrlen, "DOCSIS");
      docsis_tree = proto_item_add_subtree (ti, ett_docsis);

/
      proto_tree_add_item (docsis_tree, hf_docsis_fctype, tvb, 0, 1, FALSE);
      switch (fctype)
	{
	case FCTYPE_PACKET:
	case FCTYPE_ATMPDU:
	case FCTYPE_RESRVD:
	  proto_tree_add_item (docsis_tree, hf_docsis_fcparm, tvb, 0, 1,
			       FALSE);
	  proto_tree_add_item (docsis_tree, hf_docsis_ehdron, tvb, 0, 1,
			       FALSE);
	  if (ehdron == 0x01)
	    {
	      proto_tree_add_item (docsis_tree, hf_docsis_ehdrlen, tvb, 1, 1,
				   FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_lensid, tvb, 2, 2,
				   FALSE);
	      dissect_ehdr (tvb, docsis_tree, isfrag);
	      proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb,
				   4 + mac_parm, 2, FALSE);
	    }
	  else
	    {
	      proto_tree_add_item (docsis_tree, hf_docsis_macparm, tvb, 1, 1,
				   FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_lensid, tvb, 2, 2,
				   FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb, 4, 2,
				   FALSE);
	    }
	  break;
	case FCTYPE_MACSPC:
	  proto_tree_add_item (docsis_tree, hf_docsis_machdr_fcparm, tvb, 0,
			       1, FALSE);
	  proto_tree_add_item (docsis_tree, hf_docsis_ehdron, tvb, 0, 1,
			       FALSE);
	  /
	  if (fcparm == 0x02)
	    {
	      proto_tree_add_uint (docsis_tree, hf_docsis_mini_slots, tvb, 1,
				   1, mac_parm);
	      proto_tree_add_uint (docsis_tree, hf_docsis_sid, tvb, 2, 2,
				   len_sid);
	      proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb, 4, 2,
				   FALSE);
	      break;
	    }
	  /
	  if (fcparm == 0x03)
	    {
	      isfrag = TRUE;
	    }
	  /
	  if (fcparm == 0x1c)
	    {
	      proto_item_append_text (ti, " (Concatenated Header)");
	      proto_tree_add_item (docsis_tree, hf_docsis_concat_cnt, tvb, 1,
				   1, FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_lensid, tvb, 2, 2,
				   FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb, 4, 2,
				   FALSE);
	      break;
	    }
	  /
	  if (ehdron == 0x01)
	    {
	      proto_tree_add_item (docsis_tree, hf_docsis_ehdrlen, tvb, 1, 1,
				   FALSE);
	      proto_tree_add_item (docsis_tree, hf_docsis_lensid, tvb, 2, 2,
				   FALSE);
	      dissect_ehdr (tvb, docsis_tree, isfrag);
	      proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb,
				   4 + mac_parm, 2, FALSE);
	      break;
	    }
	  /
	  proto_tree_add_item (docsis_tree, hf_docsis_macparm, tvb, 1, 1,
			       FALSE);
	  proto_tree_add_item (docsis_tree, hf_docsis_lensid, tvb, 2, 2,
			       FALSE);
	  proto_tree_add_item (docsis_tree, hf_docsis_hcs, tvb, 4, 2, FALSE);
	  break;
	}
    }

/
  switch (fctype)
    {
    case FCTYPE_PACKET:
      if (pdulen >= 0)
	{
	  if (pdulen > 0)
	    {
	      next_tvb = tvb_new_subset (tvb, hdrlen, captured_length, pdulen);
	      call_dissector (eth_withoutfcs_handle, next_tvb, pinfo, tree);
	    }
	  if (concatlen > 0)
	    {
	      concatlen = concatlen - framelen;
	      concatpos += framelen;
	    }
	}
      break;
    case FCTYPE_MACSPC:
      switch (fcparm)
	{
	case 0x00:
	case 0x01:
	  if (pdulen > 0)
	    {
	      mgt_tvb = tvb_new_subset (tvb, hdrlen, captured_length, pdulen);
	      call_dissector (docsis_mgmt_handle, mgt_tvb, pinfo, tree);
	    }
	  if (concatlen > 0)
	    {
	      concatlen = concatlen - framelen;
	      concatpos += framelen;
	    }
	  break;
	case 0x02:
	  /
	  break;
	case 0x03:
	  /
	  if (pdulen > 0)
	    {
	      mgt_tvb = tvb_new_subset (tvb, hdrlen, captured_length, pdulen);
	      call_dissector (data_handle, mgt_tvb, pinfo, tree);
	    }
	  if (concatlen > 0)
	    {
	      concatlen = concatlen - framelen;
	      concatpos += framelen;
	    }
	  break;
	case 0x1c:
	  /
	  while (concatlen > 0)
	    {
	      next_tvb = tvb_new_subset (tvb, concatpos, -1, concatlen);
	      call_dissector (docsis_handle, next_tvb, pinfo, tree);
	    }
	  concatlen = 0;
	  concatpos = 0;
	  if (check_col (pinfo->cinfo, COL_INFO))
	    col_set_str (pinfo->cinfo, COL_INFO, "Concatenated Frame");
	  break;
	}
      break;
    }
}
