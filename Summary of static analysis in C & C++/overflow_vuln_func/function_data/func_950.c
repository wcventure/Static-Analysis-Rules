static void
dissect_ehdr (tvbuff_t * tvb, proto_tree * tree, gboolean isfrag)
{
  proto_tree *ehdr_tree;
  proto_item *it;
  guint8 ehdrlen;
  int pos;
  guint8 type;
  guint8 len;
  guint8 val;
  guint8 mini_slots;
  guint16 sid;

  ehdrlen = tvb_get_guint8 (tvb, 1);
  pos = 4;

  it = proto_tree_add_text (tree, tvb, pos, ehdrlen, "Extended Header");
  ehdr_tree = proto_item_add_subtree (it, ett_ehdr);
  while (pos < (int)(ehdrlen + 4))
    {
      type = (tvb_get_guint8 (tvb, pos) & 0xF0);
      len = (tvb_get_guint8 (tvb, pos) & 0x0F);
      if ((((type >> 4) & 0x0F)== 6) && (len == 2)) 
        {
          proto_tree_add_item_hidden (ehdr_tree, hf_docsis_eh_type, tvb, pos, 1, FALSE);
	  proto_tree_add_text(ehdr_tree, tvb, pos, 1, "0110 ....  = Unsolicited Grant Sync EHDR Sub-Element" );
        }
      else
        proto_tree_add_item (ehdr_tree, hf_docsis_eh_type, tvb, pos, 1, FALSE);
      proto_tree_add_item (ehdr_tree, hf_docsis_eh_len, tvb, pos, 1, FALSE);
      switch ((type >> 4) & 0x0F)
	{
	case EH_REQUEST:
	  if (len == 3)
	    {
	      mini_slots = tvb_get_guint8 (tvb, pos + 1);
	      sid = tvb_get_ntohs (tvb, pos + 2);
	      proto_tree_add_uint (ehdr_tree, hf_docsis_mini_slots, tvb,
				   pos + 1, 1, mini_slots);
	      proto_tree_add_uint (ehdr_tree, hf_docsis_sid, tvb, pos + 2, 2,
				   sid);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	  break;
	case EH_ACK_REQ:
	  if (len == 2)
	    {
	      sid = tvb_get_ntohs (tvb, pos + 1);
	      proto_tree_add_uint (ehdr_tree, hf_docsis_sid, tvb, pos + 2, 2,
				   sid);
	    }
	  else
	    {
	      THROW (ReportedBoundsError);
	    }
	case EH_BP_UP:
	  proto_tree_add_item (ehdr_tree, hf_docsis_key_seq, tvb, pos + 1, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_ehdr_ver, tvb, pos + 1, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_bpi_en, tvb, pos + 2, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_toggle_bit, tvb, pos + 2,
			       1, FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_sid, tvb, pos + 2, 2,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_mini_slots, tvb, pos + 4,
			       1, FALSE);
	  if (isfrag)
	    {
	      proto_tree_add_item (ehdr_tree, hf_docsis_frag_rsvd, tvb, pos+5,
	                          1, FALSE);
	      proto_tree_add_item (ehdr_tree, hf_docsis_frag_first, tvb, pos+5,
			          1, FALSE);
	      proto_tree_add_item (ehdr_tree, hf_docsis_frag_last, tvb, pos+5,
			          1, FALSE);
	      proto_tree_add_item (ehdr_tree, hf_docsis_frag_seq, tvb, pos+5,
			          1, FALSE);
	    }
	  break;
	case EH_BP_DOWN:
	  proto_tree_add_item (ehdr_tree, hf_docsis_key_seq, tvb, pos + 1, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_ehdr_ver, tvb, pos + 1, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_bpi_en, tvb, pos + 2, 1,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_toggle_bit, tvb, pos + 2,
			       1, FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_said, tvb, pos + 2, 2,
			       FALSE);
	  proto_tree_add_item (ehdr_tree, hf_docsis_reserved, tvb, pos + 4, 1,
			       FALSE);
	  break;
	case EH_SFLOW_HDR_DOWN:
	case EH_SFLOW_HDR_UP:
	  val = tvb_get_guint8 (tvb, pos+1);
	  if (val == 0)
	  {
	    proto_tree_add_item_hidden (ehdr_tree, hf_docsis_ehdr_phsi, tvb, pos+1, 1, FALSE);
	    proto_tree_add_text (ehdr_tree, tvb, pos+1, 1, "0000 0000 = No PHS on current packet" );
	  }
	  else
	    proto_tree_add_item(ehdr_tree, hf_docsis_ehdr_phsi, tvb, pos+1, 1, FALSE);

	  if (len == 2) 
	  {
	    proto_tree_add_item (ehdr_tree, hf_docsis_ehdr_qind, tvb, pos+2, 1, FALSE);
	    proto_tree_add_item (ehdr_tree, hf_docsis_ehdr_grants, tvb, pos+2, 1, FALSE);
	  }
	  break;
	default:
	  if (len > 0)
	    proto_tree_add_item (ehdr_tree, hf_docsis_eh_val, tvb, pos + 1,
				  len, FALSE);
	}
      pos += len + 1;
    }

  return;
}
