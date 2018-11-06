static void
dissect_tlv (tvbuff_t * tvb, packet_info * pinfo _U_, proto_tree * tree)
{

  proto_item *it;
  proto_tree *tlv_tree;
  guint16 total_len;
  guint16 pos = 0;
  guint8 type, length;
  guint16 x;
  tvbuff_t *vsif_tvb;

  total_len = tvb_length_remaining (tvb, 0);


  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_tlv, tvb, 0,
					total_len, "TLV Data");
      tlv_tree = proto_item_add_subtree (it, ett_docsis_tlv);
      while (pos < total_len)
	{
	  type = tvb_get_guint8 (tvb, pos++);
	  length = tvb_get_guint8 (tvb, pos++);
	  switch (type)
	    {
	    case TLV_DOWN_FREQ:
	      /
	      if (length == 4)
		proto_tree_add_item (tlv_tree, hf_docsis_tlv_down_freq, tvb,
				     pos, length, FALSE);
	      else if (length == 1)
		proto_tree_add_item (tlv_tree, hf_docsis_tlv_rng_tech, tvb,
				     pos, length, FALSE);
	      else
		dissect_doc10cos (tvb, tlv_tree, pos, length);
	      break;
	    case TLV_CHNL_ID:
	      if (length == 1)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_upstream_chid,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_NET_ACCESS:
	      if (length == 1)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_net_access,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_COS:
	      dissect_cos (tvb, tlv_tree, pos, length);
	      break;
	    case TLV_MODEM_CAP:
	      dissect_modemcap (tvb, tlv_tree, pos, length);
	      break;
	    case TLV_CM_MIC:
	      if (length == 16)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_cm_mic, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_CMTS_MIC:
	      if (length == 16)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_cmts_mic, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_VENDOR_ID:
	      if (length == 3)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_vendor_id, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_SW_UPG_FILE:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_sw_file, tvb, pos,
				   length, FALSE);
	      break;
	    case TLV_SNMP_WRITE_CTRL:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_snmp_access, tvb,
			          pos, length, FALSE);
	      break;
	    case TLV_SNMP_OBJECT:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_snmp_obj, tvb,
			          pos, length, FALSE);
	      break;
	    case TLV_MODEM_IP:
	      if (length == 4)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_modem_addr,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_SVC_UNAVAIL:
	      if (length == 3)
	        {
	          dissect_svc_unavail(tvb, tlv_tree, pos, length);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_ETHERNET_MAC:
	      if (length == 6)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_cpe_ethernet,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_TEL_SETTINGS:
	      break;
	    case TLV_BPI_CONFIG:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_bpi, tvb,
				   pos, length, FALSE);
	      break;
	    case TLV_MAX_CPES:
	      if (length == 1)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_max_cpe, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_TFTP_TIME:
	      if (length == 4)
		{
		  proto_tree_add_item (tlv_tree,
				       hf_docsis_tlv_tftp_server_timestamp,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_TFTP_MODEM_ADDRESS:
	      if (length == 4)
		{
		  proto_tree_add_item (tlv_tree,
				       hf_docsis_tlv_tftp_prov_modem_address,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_SW_UPG_SRVR:
	      if (length == 4)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_sw_upg_srvr,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_UPSTREAM_CLASSIFIER:
	    case TLV_DOWN_CLASSIFIER:
	      dissect_classifiers (tvb, tlv_tree, pos, length, type);
	      break;
	    case TLV_UPSTREAM_SERVICE_FLOW:
	    case TLV_DOWN_SERVICE_FLOW:
	      dissect_sflow (tvb, tlv_tree, pos, length, type);
	      break;
	    case TLV_PHS:
	      dissect_phs (tvb, tlv_tree, pos, length);
	      break;
	    case TLV_HMAC_DIGEST:
	      if (length == 20)
	        {
		  proto_tree_add_item (tlv_tree,
				      hf_docsis_tlv_hmac_digest, tvb,
				      pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_MAX_CLASSIFIERS:
	      if (length == 2)
		{
		  proto_tree_add_item (tlv_tree,
				       hf_docsis_tlv_max_classifiers, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_PRIVACY_ENABLE:
	      if (length == 1)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_privacy_enable,
				       tvb, pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_AUTH_BLOCK:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_auth_block,
		                   tvb, pos, length, FALSE);
	      break;
	    case TLV_KEY_SEQ_NUM:
	      if (length == 1)
		{
		  proto_tree_add_item (tlv_tree, hf_docsis_tlv_key_seq_num, tvb,
				       pos, length, FALSE);
		}
	      else
		{
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_MFGR_CVC:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_mfgr_cvc,
		                   tvb, pos, length, FALSE);
	      break;
	    case TLV_COSIGN_CVC:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_cosign_cvc,
		                   tvb, pos, length, FALSE);
	      break;
	    case TLV_SNMPV3_KICKSTART:
	      dissect_snmpv3_kickstart(tvb, tlv_tree, pos, length);
	      break;
	    case TLV_SUBS_MGMT_CTRL:
	      proto_tree_add_item (tlv_tree, hf_docsis_tlv_subs_mgmt_ctrl,
		                   tvb, pos, length, FALSE);
	      break;
	    case TLV_SUBS_MGMT_CPE:
	      if ((length % 4) == 0)
	        {
	        proto_tree_add_item (tlv_tree, hf_docsis_tlv_subs_mgmt_ip_table,
		                   tvb, pos, length, FALSE);
	        for (x = 0; x < length; x+=4)
		  {
	          proto_tree_add_item (tlv_tree,
				       hf_docsis_tlv_subs_mgmt_ip_entry,
		                       tvb, pos + x, 4, FALSE);
		  }
	        }
	      else
	        {
		  THROW (ReportedBoundsError);
		}
	      break;
	    case TLV_SUBS_MGMT_FLTR:
	      proto_tree_add_item (tlv_tree,
			           hf_docsis_tlv_subs_mgmt_filter_grps,
		                   tvb, pos, length, FALSE);
	      break;
	    case TLV_VENDOR_SPEC:
	      vsif_tvb = tvb_new_subset (tvb, pos, length, length);
	      call_dissector (docsis_vsif_handle, vsif_tvb, pinfo, tlv_tree);
	      break;
	    case TLV_END:
	      break;
	    }			/

	  pos = pos + length;
	}			/
    }				/



}
