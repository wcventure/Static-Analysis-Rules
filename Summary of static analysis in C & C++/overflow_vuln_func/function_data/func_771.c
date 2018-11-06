static int
add_tagged_field (packet_info * pinfo, proto_tree * tree, tvbuff_t * tvb, int offset)
{
  guint32 oui;
  const guint8 *tag_val;
  const guint8 *tag_data_ptr;
  guint32 tag_no, tag_len;
  unsigned int i;
  int n, ret;
  char out_buff[SHORT_STR];
  char print_buff[SHORT_STR];
  proto_tree * orig_tree=tree;
  proto_item *ti;

  tag_no = tvb_get_guint8(tvb, offset);
  tag_len = tvb_get_guint8(tvb, offset + 1);

  ti=proto_tree_add_text(orig_tree,tvb,offset,tag_len+2,"%s",
                         val_to_str(tag_no, tag_num_vals,
                         (tag_no >= 17 && tag_no <= 31) ?
                         "Reserved for challenge text" : "Reserved tag number" ));
  tree=proto_item_add_subtree(ti,ett_80211_mgt_ie);

  proto_tree_add_uint_format (tree, tag_number, tvb, offset, 1, tag_no,
			      "Tag Number: %u (%s)",
			      tag_no,
			      val_to_str(tag_no, tag_num_vals,
					 (tag_no >= 17 && tag_no <= 31) ?
					 "Reserved for challenge text" :
					 "Reserved tag number"));
  proto_tree_add_uint (tree, (tag_no==TAG_TIM ? tim_length : tag_length), tvb, offset + 1, 1, tag_len);

  switch (tag_no)
    {

    case TAG_SSID:
      if(beacon_padding == 0) /
      {
        char *ssid; /

        ssid = tvb_get_ephemeral_string(tvb, offset + 2, tag_len);
        proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
                               tag_len, ssid);
        if (check_col (pinfo->cinfo, COL_INFO)) {
          if (tag_len > 0) {
            col_append_fstr(pinfo->cinfo, COL_INFO, ", SSID: \"%s\"",
                            format_text(ssid, tag_len));
          } else {
            col_append_fstr(pinfo->cinfo, COL_INFO, ", SSID: Broadcast");
          }
        }
        if (tag_len > 0) {
          proto_item_append_text(ti, ": \"%s\"",
                                 format_text(ssid, tag_len));
        } else {
          proto_item_append_text(ti, ": Broadcast");
        }
	beacon_padding++; /
      }
      break;

    case TAG_SUPP_RATES:
    case TAG_EXT_SUPP_RATES:
      if (tag_len < 1)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"Tag length %u too short, must be > 0", tag_len);
        break;
      }

      tag_data_ptr = tvb_get_ptr (tvb, offset + 2, tag_len);
      for (i = 0, n = 0; i < tag_len && n < SHORT_STR; i++) {
        ret = g_snprintf (print_buff + n, SHORT_STR - n, "%2.1f%s ",
                        (tag_data_ptr[i] & 0x7F) * 0.5,
                        (tag_data_ptr[i] & 0x80) ? "(B)" : "");
        if (ret == -1 || ret >= SHORT_STR - n) {
          /
          break;
        }
        n += ret;
      }
      g_snprintf (out_buff, SHORT_STR, "Supported rates: %s [Mbit/sec]", print_buff);
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
			     tag_len, out_buff);
      proto_item_append_text(ti, ": %s", print_buff);
      break;

    case TAG_FH_PARAMETER:
      if (tag_len < 5)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 5",
                             tag_len);
        break;
      }
      g_snprintf (out_buff, SHORT_STR,
		"Dwell time 0x%04X, Hop Set %2d, Hop Pattern %2d, Hop Index %2d",
		tvb_get_letohs(tvb, offset + 2),
		tvb_get_guint8(tvb, offset + 4),
		tvb_get_guint8(tvb, offset + 5),
		tvb_get_guint8(tvb, offset + 6));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
                             tag_len, out_buff);
      break;

    case TAG_DS_PARAMETER:
      if (tag_len < 1)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 1",
                             tag_len);
        break;
      }
      g_snprintf (out_buff, SHORT_STR, "Current Channel: %u",
                tvb_get_guint8(tvb, offset + 2));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
                             tag_len, out_buff);
      proto_item_append_text(ti, ": %s", out_buff);
      break;

    case TAG_CF_PARAMETER:
      if (tag_len < 6)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 6",
                             tag_len);
        break;
      }
      g_snprintf (out_buff, SHORT_STR, "CFP count: %u",
                tvb_get_guint8(tvb, offset + 2));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 2,
                                   1, out_buff, "%s", out_buff);
      g_snprintf (out_buff, SHORT_STR, "CFP period: %u",
                tvb_get_guint8(tvb, offset + 3));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 3,
                                   1, out_buff, "%s", out_buff);
      g_snprintf (out_buff, SHORT_STR, "CFP max duration: %u",
                tvb_get_letohs(tvb, offset + 4));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 4,
                                   2, out_buff, "%s", out_buff);
      g_snprintf (out_buff, SHORT_STR, "CFP Remaining: %u",
                tvb_get_letohs(tvb, offset + 6));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 6,
                                   2, out_buff, "%s", out_buff);
      proto_item_append_text(ti, ": CFP count %u, CFP period %u, CFP max duration %u, "
                             "CFP Remaining %u",
                             tvb_get_guint8(tvb, offset + 2),
                             tvb_get_guint8(tvb, offset + 3),
                             tvb_get_letohs(tvb, offset + 4),
                             tvb_get_letohs(tvb, offset + 6));
      break;

    case TAG_TIM:
      if (tag_len < 4)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 4",
                             tag_len);
        break;
      }
      {
        guint8 bmapctl;
        guint8 bmapoff;
        guint8 bmaplen;
        const guint8* bmap;

        proto_tree_add_item(tree, tim_dtim_count, tvb,
                            offset + 2, 1, TRUE);
        proto_tree_add_item(tree, tim_dtim_period, tvb,
                            offset + 3, 1, TRUE);
        proto_item_append_text(ti, ": DTIM %u of %u bitmap",
                               tvb_get_guint8(tvb, offset + 2),
                               tvb_get_guint8(tvb, offset + 3));

        bmapctl = tvb_get_guint8(tvb, offset + 4);
        bmapoff = bmapctl>>1;
        proto_tree_add_uint_format(tree, tim_bmapctl, tvb,
                            offset + 4, 1, bmapctl,
                            "Bitmap Control: 0x%02X (mcast:%u, bitmap offset %u)",
                            bmapctl, bmapctl&1, bmapoff);

        bmaplen = tag_len - 3;
        bmap = tvb_get_ptr(tvb, offset + 5, bmaplen);
        if (bmaplen==1 && 0==bmap[0] && !(bmapctl&1)) {
          proto_item_append_text(ti, " empty");
        } else {
          if (bmapctl&1) {
            proto_item_append_text(ti, " mcast");
          }
        }
        if (bmaplen>1 || bmap[0]) {
          int len=g_snprintf (out_buff, SHORT_STR,
                            "Bitmap: traffic for AID's:");
          int i=0;
          for (i=0;i<bmaplen*8;i++) {
            if (bmap[i/8] & (1<<(i%8))) {
              int aid=i+2*bmapoff*8;
              len+=g_snprintf (out_buff+len, SHORT_STR-len," %u", aid);
              proto_item_append_text(ti, " %u", aid);
              if (len>=SHORT_STR) {
                break;
              }
            }
          }
          out_buff[SHORT_STR-1] = '\0';
          proto_tree_add_string_format (tree, tag_interpretation, tvb, offset + 5,
               bmaplen, out_buff, "%s", out_buff);
        }
      }
      break;

    case TAG_IBSS_PARAMETER:
      if (tag_len < 2)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 2",
                             tag_len);
        break;
      }
      g_snprintf (out_buff, SHORT_STR, "ATIM window 0x%X",
                tvb_get_letohs(tvb, offset + 2));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
			     tag_len, out_buff);
      proto_item_append_text(ti, ": %s", out_buff);
      break;

    case TAG_COUNTRY_INFO: /
      {
        guint8 ccode[2+1];

        if (tag_len < 3)
        {
          proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 3",
                               tag_len);
          break;
        }
        tvb_memcpy(tvb, ccode, offset + 2, 2);
        ccode[2] = '\0';
        g_snprintf (out_buff, SHORT_STR, "Country Code: %s, %s Environment",
                 format_text(ccode, 2),
                 val_to_str(tvb_get_guint8(tvb, offset + 4), environment_vals,"Unknown (0x%02x)"));
        out_buff[SHORT_STR-1] = '\0';
        proto_item_append_text(ti, ": %s", out_buff);
        proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,3, out_buff);

        for (i = 3; (i + 3) <= tag_len; i += 3)
        {
	  guint8 val1, val2, val3;
	  val1 = tvb_get_guint8(tvb, offset + 2 + i);
	  val2 = tvb_get_guint8(tvb, offset + 3 + i);
	  val3 = tvb_get_guint8(tvb, offset + 4 + i);

	  if (val1 <= 200) {  /
            proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 2+i,3, out_buff,
                                       "  Start Channel: %u, Channels: %u, Max TX Power: %d dBm",
                                       val1, val2, (gint) val3);
	  } else {  /
            proto_tree_add_string_format(tree, tag_interpretation, tvb, offset + 2+i,3, out_buff,
                                       "  Reg Extension Id: %u, Regulatory Class: %u, Coverage Class: %u",
                                       val1, val2, val3);
	  }
        }
      }
      break;

    case TAG_QBSS_LOAD:
      if (tag_len < 4 || tag_len >5)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Wrong QBSS Tag Length %u", tag_len);
        break;
      }

      if (tag_len == 4)
      {
        /
        proto_tree_add_string (tree, tag_interpretation, tvb, offset + 1,
          tag_len, "Cisco QBSS Version 1 - non CCA");

        /
        proto_tree_add_uint (tree, hf_qbss_version, tvb, offset + 2, tag_len, 1);
        proto_tree_add_item (tree, hf_qbss_scount, tvb, offset + 2, 2, TRUE);
        proto_tree_add_item (tree, hf_qbss_cu, tvb, offset + 4, 1, FALSE);
        proto_tree_add_item (tree, hf_qbss_adc, tvb, offset + 5, 1, FALSE);
      }
      else if (tag_len == 5)
      {
         /
         proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
           tag_len, "802.11e CCA Version");

         /
         proto_tree_add_uint (tree, hf_qbss_version, tvb, offset + 2, tag_len, 2);
         proto_tree_add_item (tree, hf_qbss_scount, tvb, offset + 2, 2, TRUE);
         proto_tree_add_item (tree, hf_qbss_cu, tvb, offset + 4, 1, FALSE);
         proto_tree_add_item (tree, hf_qbss_adc, tvb, offset + 5, 2, FALSE);
      }
      break;

    case TAG_FH_HOPPING_PARAMETER:
      if (tag_len < 2)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 2",
                             tag_len);
        break;
      }
      g_snprintf (out_buff, SHORT_STR, "Prime Radix: %u, Number of Channels: %u",
                tvb_get_guint8(tvb, offset + 2),
                tvb_get_guint8(tvb, offset + 3));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2, tag_len, out_buff);
      proto_item_append_text(ti, ": %s", out_buff);
      break;

    case TAG_TSPEC:
      if (tag_len != 55)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"TSPEC tag length %u != 55", tag_len);
	break;
      }
      add_fixed_field (tree, tvb, offset + 2, FIELD_QOS_TS_INFO);
      proto_tree_add_item(tree, tspec_nor_msdu, tvb, offset + 5, 2, TRUE);
      proto_tree_add_item(tree, tspec_max_msdu, tvb, offset + 7, 2, TRUE);
      proto_tree_add_item(tree, tspec_min_srv, tvb, offset + 9, 4, TRUE);
      proto_tree_add_item(tree, tspec_max_srv, tvb, offset + 13, 4, TRUE);
      proto_tree_add_item(tree, tspec_inact_int, tvb, offset + 17, 4, TRUE);
      proto_tree_add_item(tree, tspec_susp_int, tvb, offset + 21, 4, TRUE);
      proto_tree_add_item(tree, tspec_srv_start, tvb, offset + 25, 4, TRUE);
      proto_tree_add_item(tree, tspec_min_data, tvb, offset + 29, 4, TRUE);
      proto_tree_add_item(tree, tspec_mean_data, tvb, offset + 33, 4, TRUE);
      proto_tree_add_item(tree, tspec_peak_data, tvb, offset + 37, 4, TRUE);
      proto_tree_add_item(tree, tspec_burst_size, tvb, offset + 41, 4, TRUE);
      proto_tree_add_item(tree, tspec_delay_bound, tvb, offset + 45, 4, TRUE);
      proto_tree_add_item(tree, tspec_min_phy, tvb, offset + 49, 4, TRUE);
      proto_tree_add_item(tree, tspec_surplus, tvb, offset + 53, 2, TRUE);
      proto_tree_add_item(tree, tspec_medium, tvb, offset + 55, 2, TRUE);
      break;

    case TAG_TS_DELAY:
      if (tag_len != 4)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"TS_DELAY tag length %u != 4", tag_len);
	break;
      }
      proto_tree_add_item(tree, ts_delay, tvb, offset + 2, 4, TRUE);
      break;

    case TAG_TCLAS:
      if (tag_len < 6)
      {
	proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"TCLAS element is too small %u", tag_len);
	break;
      }
      {
	guint8 type;
	guint8 version;

	type = tvb_get_guint8(tvb, offset + 2);
	proto_tree_add_item(tree, hf_tsinfo_up, tvb, offset + 2, 1, TRUE);
	proto_tree_add_item(tree, hf_class_type, tvb, offset + 3, 1, TRUE);
	proto_tree_add_item(tree, hf_class_mask, tvb, offset + 4, 1, TRUE);
	switch (type)
	  {
	    case 0:
	      proto_tree_add_item(tree, ff_src_mac_addr, tvb, offset + 5,
	      			  6, TRUE);
	      proto_tree_add_item(tree, ff_dst_mac_addr, tvb, offset + 11,
	      			  6, TRUE);
	      proto_tree_add_item(tree, hf_ether_type, tvb, offset + 17,
	      			  2, TRUE);
	      break;

	    case 1:
	      version = tvb_get_guint8(tvb, offset + 5);
	      proto_tree_add_item(tree, cf_version, tvb, offset + 5, 1, TRUE);
	      if (version == 4)
	      {
	        proto_tree_add_item(tree, cf_ipv4_src, tvb, offset + 6,
				    4, FALSE);
	        proto_tree_add_item(tree, cf_ipv4_dst, tvb, offset + 10,
				    4, FALSE);
	        proto_tree_add_item(tree, cf_src_port, tvb, offset + 14,
				    2, FALSE);
	        proto_tree_add_item(tree, cf_dst_port, tvb, offset + 16,
				    2, FALSE);
	        proto_tree_add_item(tree, cf_dscp, tvb, offset + 18,
				    1, FALSE);
	        proto_tree_add_item(tree, cf_protocol, tvb, offset + 19,
				    1, FALSE);
	      }
	      else if (version == 6)
	      {
	        proto_tree_add_item(tree, cf_ipv6_src, tvb, offset + 6,
				    16, FALSE);
	        proto_tree_add_item(tree, cf_ipv6_dst, tvb, offset + 22,
				    16, FALSE);
	        proto_tree_add_item(tree, cf_src_port, tvb, offset + 38,
				    2, FALSE);
	        proto_tree_add_item(tree, cf_dst_port, tvb, offset + 40,
				    2, FALSE);
	        proto_tree_add_item(tree, cf_flow, tvb, offset + 42,
				    3, FALSE);
	      }
	      break;

	    case 2:
	      proto_tree_add_item(tree, cf_tag_type, tvb, offset + 5,
	      			  2, TRUE);
	      break;

	    default:
	      break;
	  }
      }
      break;

    case TAG_TCLAS_PROCESS:
      if (tag_len != 1)
      {
	proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"TCLAS_PROCESS element length %u != 1", tag_len);
	break;
      }
      proto_tree_add_item(tree, hf_tclas_process, tvb, offset + 2, 1, TRUE);
      break;

    case TAG_SCHEDULE:
      if (tag_len != 14)
      {
	proto_tree_add_text (tree, tvb, offset + 2, tag_len,
		"TCLAS_PROCESS element length %u != 14", tag_len);
	break;
      }
      add_fixed_field (tree, tvb, offset + 2, FIELD_SCHEDULE_INFO);
      proto_tree_add_item(tree, hf_sched_srv_start, tvb, offset + 4, 4, TRUE);
      proto_tree_add_item(tree, hf_sched_srv_int, tvb, offset + 8, 4, TRUE);
      proto_tree_add_item(tree, hf_sched_spec_int, tvb, offset + 12, 2, TRUE);
      break;

    case TAG_CHALLENGE_TEXT:
      g_snprintf (out_buff, SHORT_STR, "Challenge text: %s",
                tvb_bytes_to_str(tvb, offset + 2, tag_len));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
			     tag_len, out_buff);
      break;

    case TAG_ERP_INFO:
    case TAG_ERP_INFO_OLD:
      {
        guint8 erp_info;

        if (tag_len < 1)
        {
          proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 1",
                               tag_len);
          break;
        }
        erp_info = tvb_get_guint8 (tvb, offset + 2);
        g_snprintf (print_buff, SHORT_STR, "%sNon-ERP STAs, %suse protection, %s preambles",
                  erp_info & 0x01 ? "" : "no ",
                  erp_info & 0x02 ? "" : "do not ",
                  /
                  erp_info & 0x04 ? "long": "short or long");
        print_buff[SHORT_STR-1] = '\0';
        g_snprintf (out_buff, SHORT_STR,
                  "ERP info: 0x%x (%s)",erp_info,print_buff);
        out_buff[SHORT_STR-1] = '\0';
        proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
                               tag_len, out_buff);
        proto_item_append_text(ti, ": %s", print_buff);
      }
      break;

    case TAG_CISCO_UNKNOWN_1:
      /
      if (tag_len < 26)
      {
        proto_tree_add_text (tree, tvb, offset + 2, tag_len, "Tag length %u too short, must be >= 26",
                             tag_len);
        break;
      }
      /
      g_snprintf (out_buff, SHORT_STR, "%.16s",
                tvb_format_stringzpad(tvb, offset + 12, 16));
      out_buff[SHORT_STR-1] = '\0';
      proto_tree_add_string_format (tree, tag_interpretation, tvb, offset + 2,
			     tag_len, "", "Tag interpretation: Unknown + Name: %s #Clients: %u",
			     out_buff,
			     /
			     tvb_get_guint8(tvb, offset + 28));
      if (check_col (pinfo->cinfo, COL_INFO)) {
          col_append_fstr(pinfo->cinfo, COL_INFO, ", Name: \"%s\"", out_buff);
      }
      break;

    case TAG_VENDOR_SPECIFIC_IE:
      tvb_ensure_bytes_exist (tvb, offset + 2, tag_len);
      if (tag_len >= 3) {
		oui = tvb_get_ntoh24(tvb, offset + 2);
		tag_val = tvb_get_ptr(tvb, offset + 2, tag_len);

#define WPAWME_OUI	0x0050F2
#define RSNOUI_VAL	0x000FAC

		switch (oui) {
		case WPAWME_OUI:
			dissect_vendor_ie_wpawme(ti, tree, tvb, offset + 2, tag_len, tag_val);
			break;
		case RSNOUI_VAL:
			dissect_vendor_ie_rsn(ti, tree, tvb, offset + 2, tag_len, tag_val);
			break;
		case OUI_CISCOWL:	/
			dissect_vendor_ie_aironet(ti, tree, tvb, offset + 5, tag_len - 3);
			break;
		default:
			proto_tree_add_bytes_format (tree, tag_oui, tvb, offset + 2, 3,
				"", "Vendor: %s", get_manuf_name(tag_val));
			proto_item_append_text(ti, ": %s", get_manuf_name(tag_val));
			proto_tree_add_string (tree, tag_interpretation, tvb, offset + 5,
				tag_len - 3, "Not interpreted");
			break;
		}

      }
      break;

    case TAG_RSN_IE:
      dissect_rsn_ie(tree, tvb, offset + 2, tag_len,
                     tvb_get_ptr (tvb, offset + 2, tag_len));
      break;

    default:
      tvb_ensure_bytes_exist (tvb, offset + 2, tag_len);
      proto_tree_add_string (tree, tag_interpretation, tvb, offset + 2,
			     tag_len, "Not interpreted");
      proto_item_append_text(ti, ": Tag %u Len %u", tag_no, tag_len);
      break;
    }

  return tag_len + 2;
}
