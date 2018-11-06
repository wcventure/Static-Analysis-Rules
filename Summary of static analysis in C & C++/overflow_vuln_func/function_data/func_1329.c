static void
dissect_attrs (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{
  guint8 type;
  guint16 length;
  guint16 pos = 0;
  guint16 total_len;
  proto_item *cmid_it, *tekp_it, *scap_it;
  proto_item *saqry_it, *dnld_it, *sadsc_it;
  proto_tree *cmid_tree, *tekp_tree, *scap_tree;
  proto_tree *saqry_tree, *dnld_tree, *sadsc_tree;
  tvbuff_t *cmid_tvb, *tekp_tvb, *scap_tvb;
  tvbuff_t *saqry_tvb, *dnld_tvb, *sadsc_tvb;

  total_len = tvb_length_remaining (tvb, 0);
  while (pos < total_len)
    {
      type = tvb_get_guint8 (tvb, pos++);
      length = tvb_get_ntohs (tvb, pos);
      pos += 2;
      switch (type)
	{
	case BPKM_RESERVED:
	  break;
	case BPKM_SERIAL_NUM:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_serial_num, tvb, pos,
			       length, FALSE);
	  break;
	case BPKM_MANUFACTURER_ID:
	  if (length == 3)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_manf_id, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_MAC_ADDR:
	  if (length == 6)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_mac_addr, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_RSA_PUB_KEY:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_rsa_pub_key, tvb, pos,
			       length, FALSE);
	  break;
	case BPKM_CM_ID:
	  cmid_it =
	    proto_tree_add_text (tree, tvb, pos, length,
				 "5 CM Identification");
	  cmid_tree =
	    proto_item_add_subtree (cmid_it, ett_docsis_bpkmattr_cmid);
	  cmid_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (cmid_tvb, pinfo, cmid_tree);
	  break;
	case BPKM_DISPLAY_STR:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_display_str, tvb, pos,
			       length, FALSE);
	  break;
	case BPKM_AUTH_KEY:
	  if ((length == 96) || (length == 128))
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_auth_key, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_TEK:
	  if (length == 8)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_tek, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_KEY_LIFETIME:
	  if (length == 4)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_key_life, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_KEY_SEQ_NUM:
	  if (length == 1)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_key_seq, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_HMAC_DIGEST:
	  if (length == 20)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_hmac_digest, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_SAID:
	  if (length == 2)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_said, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_TEK_PARAM:
	  tekp_it =
	    proto_tree_add_text (tree, tvb, pos, length, "13 TEK Parameters");
	  tekp_tree =
	    proto_item_add_subtree (tekp_it, ett_docsis_bpkmattr_tekp);
	  tekp_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (tekp_tvb, pinfo, tekp_tree);
	  break;
	case BPKM_OBSOLETED:
	  break;
	case BPKM_CBC_IV:
	  if (length == 8)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_cbc_iv, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_ERROR_CODE:
	  if (length == 1)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_error_code, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_CA_CERT:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_ca_cert, tvb, pos,
			       length, FALSE);
	  break;
	case BPKM_CM_CERT:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_cm_cert, tvb, pos,
			       length, FALSE);
	  break;
	case BPKM_SEC_CAPABILITIES:
	  scap_it =
	    proto_tree_add_text (tree, tvb, pos, length,
				 "19 Security Capabilities");
	  scap_tree =
	    proto_item_add_subtree (scap_it, ett_docsis_bpkmattr_scap);
	  scap_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (scap_tvb, pinfo, scap_tree);
	  break;
	case BPKM_CRYPTO_SUITE:
	  if (length == 2)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_crypto_suite, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_CRYPTO_SUITE_LIST:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_crypto_suite_list,
			       tvb, pos, length, FALSE);
	  break;
	case BPKM_BPI_VERSION:
	  if (length == 1)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_bpi_version, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_SA_DESCRIPTOR:
	  sadsc_it =
	    proto_tree_add_text (tree, tvb, pos, length, "23 SA Descriptor");
	  sadsc_tree =
	    proto_item_add_subtree (sadsc_it, ett_docsis_bpkmattr_sadsc);
	  sadsc_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (sadsc_tvb, pinfo, sadsc_tree);
	  break;
	case BPKM_SA_TYPE:
	  if (length == 1)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_sa_type, tvb, pos,
				 length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_SA_QUERY:
	  saqry_it =
	    proto_tree_add_text (tree, tvb, pos, length, "25 SA Query");
	  saqry_tree =
	    proto_item_add_subtree (saqry_it, ett_docsis_bpkmattr_saqry);
	  saqry_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (saqry_tvb, pinfo, saqry_tree);
	  break;
	case BPKM_SA_QUERY_TYPE:
	  if (length == 1)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_sa_query_type, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_IP_ADDRESS:
	  if (length == 4)
	    proto_tree_add_item (tree, hf_docsis_bpkmattr_ip_address, tvb,
				 pos, length, FALSE);
	  else
	    THROW (ReportedBoundsError);
	  break;
	case BPKM_VENDOR_DEFINED:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_vendor_def, tvb, pos,
			       length, FALSE);
	case BPKM_DNLD_PARAMS:
	  dnld_it =
	    proto_tree_add_text (tree, tvb, pos, length,
				 "28 Download Parameters");
	  dnld_tree =
	    proto_item_add_subtree (dnld_it, ett_docsis_bpkmattr_dnld);
	  dnld_tvb = tvb_new_subset (tvb, pos, length, length);
	  dissect_attrs (dnld_tvb, pinfo, dnld_tree);
	  break;
	default:
	  proto_tree_add_item (tree, hf_docsis_bpkmattr_vendor_def, tvb, pos,
			       length, FALSE);
	  break;
	}
      pos += length;		/
    }				/
}
