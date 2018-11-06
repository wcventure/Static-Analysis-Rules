static void
dissect_vendor_ie_wpawme(proto_tree * ietree, proto_tree * tree, tvbuff_t * tvb,
	int offset, guint32 tag_len, const guint8 *tag_val)
{
      guint32 tag_val_off = 0;
      char out_buff[SHORT_STR];
      guint i;

      /
      if (tag_val_off + 6 <= tag_len && !memcmp(tag_val, WPA_OUI"\x01", 4)) {
        g_snprintf(out_buff, SHORT_STR, "WPA IE, type %u, version %u",
                  tag_val[tag_val_off + 3], pletohs(&tag_val[tag_val_off + 4]));
        proto_tree_add_string(tree, tag_interpretation, tvb, offset, 6, out_buff);
        offset += 6;
        tag_val_off += 6;
        if (tag_val_off + 4 <= tag_len) {
          /
          if (!memcmp(&tag_val[tag_val_off], WPA_OUI, 3)) {
            g_snprintf(out_buff, SHORT_STR, "Multicast cipher suite: %s",
                      wpa_cipher_idx2str(tag_val[tag_val_off + 3]));
            proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
            offset += 4;
            tag_val_off += 4;
            /
            if (tag_val_off + 2 <= tag_len) {
              g_snprintf(out_buff, SHORT_STR, "# of unicast cipher suites: %u",
                        pletohs(tag_val + tag_val_off));
              proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
              offset += 2;
              tag_val_off += 2;
              i = 1;
              while (tag_val_off + 4 <= tag_len) {
                if (!memcmp(&tag_val[tag_val_off], WPA_OUI, 3)) {
                  g_snprintf(out_buff, SHORT_STR, "Unicast cipher suite %u: %s",
                            i, wpa_cipher_idx2str(tag_val[tag_val_off + 3]));
                  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
                  offset += 4;
                  tag_val_off += 4;
                  i ++;
                }
                else
                  break;
              }
	      /
              if (tag_val_off + 2 <= tag_len) {
                g_snprintf(out_buff, SHORT_STR, "# of auth key management suites: %u",
                          pletohs(tag_val + tag_val_off));
                proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
                offset += 2;
                tag_val_off += 2;
                i = 1;
                while (tag_val_off + 4 <= tag_len) {
                  if (!memcmp(&tag_val[tag_val_off], WPA_OUI, 3)) {
                    g_snprintf(out_buff, SHORT_STR, "auth key management suite %u: %s",
                              i, wpa_keymgmt_idx2str(tag_val[tag_val_off + 3]));
                    proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
                    offset += 4;
                    tag_val_off += 4;
                    i ++;
                  }
                  else
                    break;
                }
              }
            }
          }
        }
        if (tag_val_off < tag_len)
          proto_tree_add_string(tree, tag_interpretation, tvb,
                                 offset, tag_len - tag_val_off, "Not interpreted");
	proto_item_append_text(ietree, ": WPA");
      } else if (tag_val_off + 7 <= tag_len && !memcmp(tag_val, WME_OUI"\x02\x00", 5)) {
      /
        g_snprintf(out_buff, SHORT_STR, "WME IE: type %u, subtype %u, version %u, parameter set %u",
		 tag_val[tag_val_off + 3], tag_val[tag_val_off + 4], tag_val[tag_val_off + 5],
		 tag_val[tag_val_off + 6]);
        proto_tree_add_string(tree, tag_interpretation, tvb, offset, 7, out_buff);
	proto_item_append_text(ietree, ": WME");
      } else if (tag_val_off + 24 <= tag_len && !memcmp(tag_val, WME_OUI"\x02\x01", 5)) {
      /
        g_snprintf(out_buff, SHORT_STR, "WME PE: type %u, subtype %u, version %u, parameter set %u",
		 tag_val[tag_val_off + 3], tag_val[tag_val_off + 4], tag_val[tag_val_off + 5],
		 tag_val[tag_val_off + 6]);
        proto_tree_add_string(tree, tag_interpretation, tvb, offset, 7, out_buff);
	offset += 8;
	tag_val_off += 8;
	for (i = 0; i < 4; i++) {
	  g_snprintf(out_buff, SHORT_STR, "WME AC Parameters: ACI %u (%s), Admission Control %sMandatory, AIFSN %u, ECWmin %u, ECWmax %u, TXOP %u",
		   (tag_val[tag_val_off] & 0x60) >> 5,
		   wme_acs[(tag_val[tag_val_off] & 0x60) >> 5],
		   (tag_val[tag_val_off] & 0x10) ? "" : "not ",
		   tag_val[tag_val_off] & 0x0f,
		   tag_val[tag_val_off + 1] & 0x0f,
		   (tag_val[tag_val_off + 1] & 0xf0) >> 4,
		   tvb_get_letohs(tvb, offset + 2));
	  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
	  offset += 4;
	  tag_val_off += 4;
	}
	proto_item_append_text(ietree, ": WME");
      } else if (tag_val_off + 56 <= tag_len && !memcmp(tag_val, WME_OUI"\x02\x02", 5)) {
      /
	guint16 ts_info, msdu_size, surplus_bandwidth;
	const char *direction[] = { "Uplink", "Downlink", "Reserved", "Bi-directional" };
	const value_string fields[] = {
	  {12, "Minimum Service Interval"},
	  {16, "Maximum Service Interval"},
	  {20, "Inactivity Interval"},
	  {24, "Service Start Time"},
	  {28, "Minimum Data Rate"},
	  {32, "Mean Data Rate"},
	  {36, "Maximum Burst Size"},
	  {40, "Minimum PHY Rate"},
	  {44, "Peak Data Rate"},
	  {48, "Delay Bound"},
	  {0, NULL}
	};
	const char *field;

        g_snprintf(out_buff, SHORT_STR, "WME TSPEC: type %u, subtype %u, version %u",
		 tag_val[tag_val_off + 3], tag_val[tag_val_off + 4], tag_val[tag_val_off + 5]);
        proto_tree_add_string(tree, tag_interpretation, tvb, offset, 6, out_buff);
	offset += 6;
	tag_val_off += 6;

	ts_info = tvb_get_letohs(tvb, offset);
	g_snprintf(out_buff, SHORT_STR, "WME TS Info: Priority %u (%s) (%s), Contention-based access %sset, %s",
		 (ts_info >> 11) & 0x7, qos_tags[(ts_info >> 11) & 0x7], qos_acs[(ts_info >> 11) & 0x7],
		 (ts_info & 0x0080) ? "" : "not ",
		 direction[(ts_info >> 5) & 0x3]);
	proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
	offset += 2;
	tag_val_off += 2;

	msdu_size = tvb_get_letohs(tvb, offset);
	g_snprintf(out_buff, SHORT_STR, "WME TSPEC: %s MSDU Size %u",
		 (msdu_size & 0x8000) ? "Fixed" : "Nominal", msdu_size & 0x7fff);
	proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
	offset += 2;
	tag_val_off += 2;

	g_snprintf(out_buff, SHORT_STR, "WME TSPEC: Maximum MSDU Size %u", tvb_get_letohs(tvb, offset));
	proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
	offset += 2;
	tag_val_off += 2;

	while ((field = val_to_str(tag_val_off, fields, "Unknown"))) {
	  g_snprintf(out_buff, SHORT_STR, "WME TSPEC: %s %u", field, tvb_get_letohl(tvb, offset));
	  proto_tree_add_string(tree, tag_interpretation, tvb, offset, 4, out_buff);
	  offset += 4;
	  tag_val_off += 4;
	  if (tag_val_off == 52)
	    break;
	}

	surplus_bandwidth = tvb_get_letohs(tvb, offset);
	g_snprintf(out_buff, SHORT_STR, "WME TSPEC: Surplus Bandwidth Allowance Factor %u.%u",
		 (surplus_bandwidth >> 13) & 0x7, (surplus_bandwidth & 0x1fff));
	offset += 2;
	tag_val_off += 2;

	g_snprintf(out_buff, SHORT_STR, "WME TSPEC: Medium Time %u", tvb_get_letohs(tvb, offset));
	proto_tree_add_string(tree, tag_interpretation, tvb, offset, 2, out_buff);
	offset += 2;
	tag_val_off += 2;
	proto_item_append_text(ietree, ": WME");
      }
}
