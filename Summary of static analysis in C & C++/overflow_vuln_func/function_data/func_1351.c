static void
dissect_ucd (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  guint16 pos, endtlvpos;
  guint8 type, length;
  guint8 tlvlen, tlvtype;
  proto_tree *ucd_tree;
  proto_item *ucd_item;
  proto_tree *tlv_tree;
  proto_item *tlv_item;
  guint16 len;
  guint8 upchid, symrate;

  len = tvb_length_remaining (tvb, 0);
  upchid = tvb_get_guint8 (tvb, 0);

  /
  if (check_col (pinfo->cinfo, COL_INFO))
    {
      col_clear (pinfo->cinfo, COL_INFO);
      if (upchid > 0)
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "UCD Message:  Channel ID = %u (U%u)", upchid,
		      upchid - 1);
      else
	col_add_fstr (pinfo->cinfo, COL_INFO,
		      "UCD Message:  Channel ID = %u (Telephony Return)",
		      upchid);
    }

  if (tree)
    {
      ucd_item =
	proto_tree_add_protocol_format (tree, proto_docsis_ucd, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"UCD Message");
      ucd_tree = proto_item_add_subtree (ucd_item, ett_docsis_ucd);
      proto_tree_add_item (ucd_tree, hf_docsis_ucd_upstream_chid, tvb, 0, 1,
			   FALSE);
      proto_tree_add_item (ucd_tree, hf_docsis_ucd_config_ch_cnt, tvb, 1, 1,
			   FALSE);
      proto_tree_add_item (ucd_tree, hf_docsis_ucd_mini_slot_size, tvb, 2, 1,
			   FALSE);
      proto_tree_add_item (ucd_tree, hf_docsis_ucd_down_chid, tvb, 3, 1,
			   FALSE);

      pos = 4;
      while (pos < len)
	{
	  type = tvb_get_guint8 (tvb, pos);
	  tlv_item = proto_tree_add_text (ucd_tree, tvb, pos, -1,
					  "%s",
					  val_to_str(type, channel_tlv_vals,
						     "Unknown TLV (%u)"));  
	  tlv_tree = proto_item_add_subtree (tlv_item, ett_tlv);
	  proto_tree_add_uint (tlv_tree, hf_docsis_ucd_type,
			       tvb, pos, 1, type);
	  pos++;
	  length = tvb_get_guint8 (tvb, pos);
	  proto_tree_add_uint (tlv_tree, hf_docsis_ucd_length,
			       tvb, pos, 1, length);
	  pos++;
	  proto_item_set_len(tlv_item, length + 2);
	  switch (type)
	    {
	    case UCD_SYMBOL_RATE:
	      if (length == 1)
		{
		  symrate = tvb_get_guint8 (tvb, pos);
		  proto_tree_add_uint (tlv_tree, hf_docsis_ucd_symbol_rate,
				       tvb, pos, length, symrate * 160);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      pos = pos + length;
	      break;
	    case UCD_FREQUENCY:
	      if (length == 4)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_ucd_frequency, tvb,
				       pos, length, FALSE);
		  pos = pos + length;
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case UCD_PREAMBLE:
	      proto_tree_add_item (tlv_tree, hf_docsis_ucd_preamble_pat, tvb,
				   pos, length, FALSE);
	      pos = pos + length;
	      break;
	    case UCD_BURST_DESCR:
	      proto_tree_add_item (tlv_tree, hf_docsis_ucd_iuc, tvb,
				   pos++, 1, FALSE);
	      endtlvpos = pos + length - 1;
	      while (pos < endtlvpos)
		{
		  tlvtype = tvb_get_guint8 (tvb, pos++);
		  tlvlen = tvb_get_guint8 (tvb, pos++);
		  switch (tlvtype)
		    {
		    case UCD_MODULATION:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_mod_type, tvb,
					       pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_DIFF_ENCODING:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_diff_encoding,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_PREAMBLE_LEN:
		      if (tlvlen == 2)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_preamble_len,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_PREAMBLE_VAL_OFF:
		      if (tlvlen == 2)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_preamble_val_off,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_FEC:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_fec, tvb, pos,
					       tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_FEC_CODEWORD:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_fec_codeword,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_SCRAMBLER_SEED:
		      if (tlvlen == 2)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_scrambler_seed,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_MAX_BURST:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_max_burst, tvb,
					       pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_GUARD_TIME:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_guard_time,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_LAST_CW_LEN:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_last_cw_len,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    case UCD_SCRAMBLER_ONOFF:
		      if (tlvlen == 1)
			{
			  proto_tree_add_item (tlv_tree,
					       hf_docsis_burst_scrambler_onoff,
					       tvb, pos, tlvlen, FALSE);
			}
		      else
			{
			  THROW (ReportedBoundsError);
			}
		      break;
		    }		/
		  pos = pos + tlvlen;
		}		/
	      break;
	    }			/
	}			/
    }				/

}
