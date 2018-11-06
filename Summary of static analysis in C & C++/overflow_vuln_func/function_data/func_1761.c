static void
dissect_phs_err (tvbuff_t * tvb, proto_tree * tree, guint16 start,
		 guint16 len)
{
  guint8 type, length;
  proto_item *it;
  proto_tree *err_tree;
  guint16 pos = start;
  it =
    proto_tree_add_text (tree, tvb, start, len,
			 "5 Service Flow Error Encodings (Length = %u)", len);
  err_tree = proto_item_add_subtree (it, ett_docsis_tlv_sflow_err);

  while (pos < (start + len))
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_guint8 (tvb, pos++);
      switch (type)
	{
	case PHS_ERR_PARAM:
	  if (length == 1)
	    {
	      proto_tree_add_item (err_tree, hf_docsis_tlv_phs_err_param, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_ERR_CODE:
	  if (length == 1)
	    {
	      proto_tree_add_item (err_tree, hf_docsis_tlv_phs_err_code, tvb,
				   pos, length, FALSE);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case PHS_ERR_MSG:
	  proto_tree_add_item (err_tree, hf_docsis_tlv_phs_err_msg, tvb, pos,
			       length, FALSE);
	  break;
	}			/
      pos = pos + length;

    }				/
}
