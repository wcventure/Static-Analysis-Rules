static void
dissect_phs (tvbuff_t * tvb, proto_tree * tree, guint16 start, guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *phs_tree;
  guint16 pos = start;
  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "26 PHS Encodings (Length = %u)", len);
  phs_tree = proto_item_add_subtree (it, ett_docsis_tlv_phs);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case PHS_CLSFR_REF:
	  if (length == 1)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_class_ref, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_CLSFR_ID:
	  if (length == 2)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_class_id, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_SFLOW_REF:
	  if (length == 2)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_sflow_ref, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_SFLOW_ID:
	  if (length == 4)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_sflow_id, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_DSC_ACTION:
	  if (length == 1)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_dsc_action,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_ERRORS:
	  dissect_phs_err (tvb, phs_tree, pos, length);
	case PHS_FIELD:
	  proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_phsf, tvb, pos,
			       length, FALSE);
	  break;
	case PHS_INDEX:
	  if (length == 1)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_phsi, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_MASK:
	  proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_phsm, tvb, pos,
			       length, FALSE);
	  break;
	case PHS_SUP_SIZE:
	  if (length == 1)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_phss, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_VERIFICATION:
	  if (length == 1)
	    {
	      proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_phsf, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_VENDOR_SPEC:
	  proto_tree_add_item (phs_tree, hf_docsis_tlv_phs_vendorspec, tvb,
			       pos, length, FALSE);
	  break;
	}			/
      pos = pos + length;

    }				/
}
