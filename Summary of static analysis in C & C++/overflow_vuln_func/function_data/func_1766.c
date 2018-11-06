static void
dissect_upstream_sflow (tvbuff_t * tvb, proto_tree * sflow_tree,
			guint16 start, guint16 len)
{
  guint8 type, length;
  guint16 pos = start;
  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case SFW_MAX_CONCAT_BURST:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_max_concat_burst, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);

	    }
	  break;
	case SFW_SCHEDULING_TYPE:
	  if (length == 1)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_sched_type,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_REQ_XMIT_POL:
	  dissect_reqxmit_policy (tvb, sflow_tree, pos);
	  break;
	case SFW_NOM_POLL_INT:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_nominal_polling, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_POLL_JTTR_TOL:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_tolerated_jitter, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_UG_SIZE:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree, hf_docsis_tlv_sflow_ugs_size,
				   tvb, pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_NOM_GRNT_INTV:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_nom_grant_intvl, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_GRNT_JTTR_TOL:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_tol_grant_jitter, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_GRNTS_PER_INTV:
	  if (length == 1)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_grants_per_intvl, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_IP_TOS_OVERWRITE:
	  if (length == 2)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_ip_tos_overwrite, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case SFW_UG_TIME_REF:
	  if (length == 4)
	    {
	      proto_tree_add_item (sflow_tree,
				   hf_docsis_tlv_sflow_ugs_timeref, tvb, pos,
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
