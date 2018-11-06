static void
dissect_cisco (tvbuff_t * tvb, proto_tree * tree, guint8 vsif_len)
{
  /
  guint16 pos = 5;
  guint8 type, length;
  proto_item *ipprec_it;
  proto_tree *ipprec_tree;
  guint16 templen;

  while (pos < vsif_len)
    {
      /
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case NUM_PHONES:
	  proto_tree_add_item (tree, hf_docsis_vsif_cisco_numphones, tvb,
			       pos, length, FALSE);
	  break;
	case IP_PREC:
	  ipprec_it =
	    proto_tree_add_text (tree, tvb, pos, length, "IP Precedence");
	  ipprec_tree =
	    proto_item_add_subtree (ipprec_it, ett_docsis_vsif_ipprec);
	  /
	  templen = pos + length;
	  while (pos < templen)
	    {
	      type = tvb_get_guint8 (tvb, pos++);
	      length = tvb_get_guint8 (tvb, pos++);
	      switch (type)
		{
		case IP_PREC_VAL:
		  if (length != 1)
		    THROW (ReportedBoundsError);
		  proto_tree_add_item (ipprec_tree,
				       hf_docsis_vsif_cisco_ipprec_val, tvb,
				       pos, length, FALSE);
		  break;
		case IP_PREC_BW:
		  if (length != 4)
		    THROW (ReportedBoundsError);
		  proto_tree_add_item (ipprec_tree,
				       hf_docsis_vsif_cisco_ipprec_bw, tvb,
				       pos, length, FALSE);
		  break;
		default:
		  THROW (ReportedBoundsError);
		}
	      pos += length;
	    }
	  break;
	case IOS_CONFIG_FILE:
	  proto_tree_add_item (tree, hf_docsis_vsif_cisco_config_file, tvb,
			       pos, length, FALSE);
	}
      pos += length;
    }

}
