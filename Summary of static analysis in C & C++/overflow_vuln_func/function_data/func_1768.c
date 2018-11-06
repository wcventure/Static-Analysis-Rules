static void
dissect_dot1q_clsfr (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		     guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *dot1qclsfr_tree;
  guint16 pos = start;
  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "11 801.1P/Q Classifiers (Length = %u)", len);
  dot1qclsfr_tree = proto_item_add_subtree (it, ett_docsis_tlv_cos);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case CFR_D1Q_USER_PRI:
	  if (length == 2)
	    {
	      proto_tree_add_item (dot1qclsfr_tree,
				   hf_docsis_tlv_dot1qclsfr_user_pri, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_D1Q_VLAN_ID:
	  if (length == 2)
	    {
	      proto_tree_add_item (dot1qclsfr_tree,
				   hf_docsis_tlv_dot1qclsfr_vlanid, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case CFR_D1Q_VENDOR_SPEC:
	  proto_tree_add_item (dot1qclsfr_tree,
			       hf_docsis_tlv_dot1qclsfr_vendorspec, tvb, pos,
			       length, FALSE);
	  break;
	}			/
      pos = pos + length;

    }				/
}
