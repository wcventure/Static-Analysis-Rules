static void
dissect_modemcap (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		  guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *mcap_tree;
  guint16 pos = start;

  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "5 Modem Capabilities Type (Length = %u)", len);

  mcap_tree = proto_item_add_subtree (it, ett_docsis_tlv_mcap);
  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case CAP_CONCAT:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_concat, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_DOCSIS_VER:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_docs_ver,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_FRAG:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_frag, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_PHS:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_phs, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_IGMP:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_igmp, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_PRIVACY:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_privacy, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_DOWN_SAID:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_down_said,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_UP_SID:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_up_sid, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_OPT_FILT:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_8021P_filter,
				   tvb, pos, length, FALSE);
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_8021Q_filter,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_XMIT_EQPERSYM:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree,
				   hf_docsis_tlv_mcap_xmit_eq_taps_per_sym,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_NUM_XMIT_EQ_TAPS:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_xmit_eq_taps,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CAP_DCC:
	  if (length == 1)
	    {
	      proto_tree_add_item (mcap_tree, hf_docsis_tlv_mcap_dcc, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	}			/
      pos = pos + length;
    }				/

}
