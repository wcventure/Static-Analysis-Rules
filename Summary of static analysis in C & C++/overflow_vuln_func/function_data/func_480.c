static void
dissect_iphc_crtp_fh(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree *fh_tree, *info_tree;
  proto_item *ti = NULL;

  guint ip_hdr_len, flags;
  guint length;
  guint hdr_len;
  tvbuff_t *next_tvb;

  int offset_seq;
  int offset_cid;

  guint8 ip_version;
  guint8 next_protocol;

  guchar* ip_packet;

  length = tvb_reported_length(tvb);

  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo,COL_PROTOCOL, "CRTP");

  if (check_col(pinfo->cinfo, COL_INFO))
    col_set_str(pinfo->cinfo, COL_INFO, "Full Header");

  /
  ip_version = tvb_get_guint8(tvb, 0) >> 4;
  next_protocol = tvb_get_guint8(tvb, 9);

  DISSECTOR_ASSERT((ip_version == 4) && (next_protocol == IP_PROTO_UDP));

  if (tree) {

    ti = proto_tree_add_protocol_format(tree, proto_iphc_crtp, tvb, 0, -1, 
					"%s", val_to_str(PPP_RTP_FH, ppp_vals, "Unknown"));
    fh_tree = proto_item_add_subtree(ti, ett_iphc_crtp);

    flags = (tvb_get_guint8(tvb, 2) & IPHC_CRTP_FH_FLAG_MASK) >> IPHC_CRTP_FH_FLAG_POS;

    /
    ip_hdr_len = (tvb_get_guint8(tvb, 0) & 0x0f) * 4;
    
    /
    hdr_len = ip_hdr_len + 8;

    /
    ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_fh_flags, tvb, 2, 1, FALSE);
    
    /
    ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_gen, tvb, 2, 1, FALSE);

    /
    switch (flags) {

    case IPHC_CRTP_FH_CID8:
      offset_cid = 3;
      offset_seq = ip_hdr_len + 5;
      ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_cid8, tvb, offset_cid, 1, FALSE);
      ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_seq, tvb, offset_seq, 1, FALSE);
      break;

    case IPHC_CRTP_FH_CID16:
      offset_seq = 3;
      offset_cid = ip_hdr_len + 4;
      ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_seq, tvb, offset_seq, 1, FALSE);
      ti = proto_tree_add_item(fh_tree, hf_iphc_crtp_cid16, tvb, offset_cid, 2, FALSE);
      break;
    }

    /
    tvb_ensure_bytes_exist (tvb, 0, length);
    ti = proto_tree_add_text(fh_tree, tvb, 0,length,"Information Field");
    info_tree = proto_item_add_subtree(ti,ett_iphc_crtp_info);

    /
    ip_packet = tvb_memdup(tvb, 0, length);

    /
    ip_packet[2] = length >> 8;
    ip_packet[3] = length;

    ip_packet[ip_hdr_len + 4] = (length - ip_hdr_len) >> 8;
    ip_packet[ip_hdr_len + 5] = (length - ip_hdr_len);

    next_tvb = tvb_new_real_data(ip_packet, length, length);
    tvb_set_child_real_data_tvbuff(tvb, next_tvb);
    add_new_data_source(pinfo, next_tvb, "Decompressed Data");
    tvb_set_free_cb(next_tvb, g_free);

    if (!dissector_try_port(ppp_subdissector_table, PPP_IP, next_tvb, pinfo, info_tree)) {
	call_dissector_only(data_handle, next_tvb, pinfo, info_tree);
    }
  } /
}
