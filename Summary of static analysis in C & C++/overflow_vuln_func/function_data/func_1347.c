static void
dissect_rngrsp (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  proto_item *it;
  proto_tree *rngrsp_tree;
  guint8 tlvtype, tlvlen;
  guint16 pos, length;
  guint8 upchid;
  guint16 sid;
  gint8 pwr;
  gint32 tim;

  sid = tvb_get_ntohs (tvb, 0);
  upchid = tvb_get_guint8 (tvb, 2);

  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      if (upchid > 0)
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "Ranging Response: SID = %u, Upstream Channel = %u (U%u)",
		      sid, upchid, upchid - 1);
      else
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "Ranging Response: SID = %u, Telephony Return", sid);

    }

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_rngrsp, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"Ranging Response");
      rngrsp_tree = proto_item_add_subtree (it, ett_docsis_rngrsp);
      proto_tree_add_item (rngrsp_tree, hf_docsis_rngrsp_sid, tvb, 0, 2,
			   FALSE);
      proto_tree_add_item (rngrsp_tree, hf_docsis_rngrsp_upstream_chid, tvb,
			   2, 1, FALSE);

      length = tvb_length_remaining (tvb, 0);
      pos = 3;
      while (pos < length)
	{
	  tlvtype = tvb_get_guint8 (tvb, pos++);
	  tlvlen = tvb_get_guint8 (tvb, pos++);
	  switch (tlvtype)
	    {
	    case RNGRSP_TIMING:
	      if (tlvlen == 4)
		{
		  tim = long_to_signed (tvb_get_ntohl (tvb, pos));
		  proto_tree_add_int (rngrsp_tree,
				      hf_docsis_rngrsp_timing_adj, tvb, pos,
				      tlvlen, tim);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case RNGRSP_PWR_LEVEL_ADJ:
	      if (tlvlen == 1)
		{
		  pwr = byte_to_signed (tvb_get_guint8 (tvb, pos));
		  proto_tree_add_int (rngrsp_tree, hf_docsis_rngrsp_power_adj,
				      tvb, pos, tlvlen, pwr);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case RNGRSP_OFFSET_FREQ_ADJ:
	      if (tlvlen == 2)
		{
		  proto_tree_add_item (rngrsp_tree, hf_docsis_rngrsp_freq_adj,
				       tvb, pos, tlvlen, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case RNGRSP_TRANSMIT_EQ_ADJ:
	      proto_tree_add_item (rngrsp_tree, hf_docsis_rngrsp_xmit_eq_adj,
				   tvb, pos, tlvlen, FALSE);
	      break;
	    case RNGRSP_RANGING_STATUS:
	      if (tlvlen == 1)
		proto_tree_add_item (rngrsp_tree,
				     hf_docsis_rngrsp_ranging_status, tvb,
				     pos, tlvlen, FALSE);
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case RNGRSP_DOWN_FREQ_OVER:
	      if (tlvlen == 4)
		proto_tree_add_item (rngrsp_tree,
				     hf_docsis_rngrsp_down_freq_over, tvb,
				     pos, tlvlen, FALSE);
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case RNGRSP_UP_CHID_OVER:
	      if (tlvlen == 1)
		proto_tree_add_item (rngrsp_tree,
				     hf_docsis_rngrsp_upstream_ch_over, tvb,
				     pos, tlvlen, FALSE);
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;

	    }			/
	  pos = pos + tlvlen;
	}			/
    }				/
}
