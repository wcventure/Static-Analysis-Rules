}
static void
dissect_classifiers (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		     guint16 len, guint8 direction)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *clsfr_tree;
  guint16 pos = start;
  if (direction == 22)
    it =
      proto_tree_add_text (tree, tvb, start, len,
			   "22 Upstream Packet Classifier (Length = %u)",
			   len);
  else if (direction == 23)
    it =
      proto_tree_add_text (tree, tvb, start, len,
			   "23 Downstream Packet Classifier (Length = %u)",
			   len);
  else
    return;
  clsfr_tree = proto_item_add_subtree (it, ett_docsis_tlv_clsfr);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case CFR_REF:
	  if (length == 1)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_ref, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_ID:
	  if (length == 2)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_id, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_SFLOW_REF:
	  if (length == 2)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_sflow_ref,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_SFLOW_ID:
	  if (length == 4)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_sflow_id,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_RULE_PRI:
	  if (length == 1)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_rule_pri,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_ACT_STATE:
	  if (length == 1)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_act_state,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_DSA_ACTION:
	  if (length == 1)
	    {
	      proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_dsc_act,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_ERROR:
	  dissect_clsfr_err (tvb, clsfr_tree, pos, length);
	  break;
	case CFR_IP_CLASSIFIER:
	  dissect_ip_classifier (tvb, clsfr_tree, pos, length);
	  break;
	case CFR_ETH_CLASSIFIER:
	  dissect_eth_clsfr (tvb, clsfr_tree, pos, length);
	  break;
	case CFR_8021Q_CLASSIFIER:
	  dissect_dot1q_clsfr (tvb, clsfr_tree, pos, length);
	  break;
	case CFR_VENDOR_SPEC:
	  proto_tree_add_item (clsfr_tree, hf_docsis_tlv_clsfr_vendor_spc,
			       tvb, pos, length, FALSE);
	  break;
	}			/
      pos = pos + length;

    }				/



}
