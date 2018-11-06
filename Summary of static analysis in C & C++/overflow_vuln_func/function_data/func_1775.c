static void
dissect_cos (tvbuff_t * tvb, proto_tree * tree, guint16 start, guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *cos_tree;
  guint16 pos = start;

  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "4 Class of Service Type (Length = %u)", len);
  cos_tree = proto_item_add_subtree (it, ett_docsis_tlv_cos);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case COS_CLASSID:
	  if (length == 1)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_id, tvb, pos,
				   length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_MAX_DOWN:
	  if (length == 4)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_max_down, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_MAX_UP:
	  if (length == 4)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_max_up, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_UP_CH_PRIO:
	  if (length == 1)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_up_chnl_pri,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_MIN_UP_RATE:
	  if (length == 4)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_min_grntd_up,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_MAX_UP_BURST:
	  if (length == 2)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_max_up_burst,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case COS_BP_ENABLE:
	  if (length == 1)
	    {
	      proto_tree_add_item (cos_tree, hf_docsis_tlv_cos_privacy_enable,
				   tvb, pos, length, FALSE);
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
