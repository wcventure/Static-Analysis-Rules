static void
dissect_ip_classifier (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		       guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *ipclsfr_tree;
  guint16 pos = start;
  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "9 IP Classifier (Length = %u)", len);
  ipclsfr_tree = proto_item_add_subtree (it, ett_docsis_tlv_clsfr_ip);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case CFR_IP_TOS_RANGE_MASK:
	  if (length == 3)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_tosmask, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_PROTO:
	  if (length == 2)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_ipproto, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_SOURCE_ADDR:
	  if (length == 4)
	    {
	      proto_tree_add_item (ipclsfr_tree, hf_docsis_tlv_ipclsfr_src,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_SOURCE_MASK:
	  if (length == 4)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_srcmask, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_DEST_ADDR:
	  if (length == 4)
	    {
	      proto_tree_add_item (ipclsfr_tree, hf_docsis_tlv_ipclsfr_dst,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_DEST_MASK:
	  if (length == 4)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_dstmask, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_SRCPORT_START:
	  if (length == 2)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_sport_start, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_SRCPORT_END:
	  if (length == 2)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_sport_end, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_DSTPORT_START:
	  if (length == 2)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_dport_start, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_IP_DSTPORT_END:
	  if (length == 2)
	    {
	      proto_tree_add_item (ipclsfr_tree,
				   hf_docsis_tlv_ipclsfr_dport_end, tvb, pos,
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
