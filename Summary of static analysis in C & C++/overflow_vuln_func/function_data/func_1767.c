static void
dissect_sflow (tvbuff_t * tvb, proto_tree * tree, guint16 start, guint16 len,
	       guint8 direction)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *sflow_tree;
  guint16 pos = start;
  if (direction == 24)
    it =
      proto_tree_add_text (tree, tvb, start, len,
			   "24 Upstream Service Flow (Length = %u)", len);
  else if (direction == 25)
    it =
      proto_tree_add_text (tree, tvb, start, len,
			   "25 Downstream Service Flow (Length = %u)", len);
  else
    return;
  sflow_tree = proto_item_add_subtree (it, ett_docsis_tlv_clsfr);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case SFW_REF:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_ref, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_ID:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_id, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_SID:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_sid, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_SERVICE_CLASS_NAME:
	  proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_classname, tvb,
			       pos, length, FALSE);
	  break;
	case SFW_ERRORS:
	  dissect_sflow_err (tvb, sflow_tree, pos, length);
	  break;
	case SFW_QOS_SET_TYPE:
	  if (length == 1)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_qos_param,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_TRAF_PRI:
	  if (length == 1)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_traf_pri,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_MAX_SUSTAINED:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_max_sus,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_MAX_BURST:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_max_burst,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_MIN_RSVD_TRAF:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_min_traf,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_MIN_RSVD_PACKETSIZE:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_ass_min_pkt_size, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_ACTIVE_QOS_TIMEOUT:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_timeout_active, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_ADMITT_QOS_TIMEOUT:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_timeout_admitted, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_VENDOR_SPEC:
	  proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_vendor_spec,
			       tvb, pos, length, FALSE);
	  break;
	default:
	  if (direction == 24)
	    dissect_upstream_sflow (tvb, sflow_tree, (guint16) (pos - 2), length);
	  else
	    dissect_downstream_sflow (tvb, sflow_tree, (guint16) (pos - 2), length);
	  break;

	}			/
      pos = pos + length;
    }				/

}
