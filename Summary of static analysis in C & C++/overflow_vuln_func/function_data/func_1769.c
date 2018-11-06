static void
dissect_eth_clsfr (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		   guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *ethclsfr_tree;
  guint16 pos = start;
  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "10 Ethernet Classifiers (Length = %u)", len);
  ethclsfr_tree = proto_item_add_subtree (it, ett_docsis_tlv_clsfr_eth);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case CFR_ETH_DST_MAC:
	  if (length == 6)
	    {
	      proto_tree_add_item (ethclsfr_tree, hf_docsis_tlv_ethclsfr_dmac,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_ETH_SRC_MAC:
	  if (length == 6)
	    {
	      proto_tree_add_item (ethclsfr_tree, hf_docsis_tlv_ethclsfr_smac,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_ETH_DSAP:
	  if (length == 3)
	    {
	      proto_tree_add_item (ethclsfr_tree,
				   hf_docsis_tlv_ethclsfr_ethertype, tvb, pos,
				   length, FALSE);
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
