static void
dissect_iphc_crtp_cs(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree *cs_tree;
  proto_item *ti = NULL;
  guint8 flags, cnt;
  guint length, cid_size;
  guint offset = 2, hf;

  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo,COL_PROTOCOL, "CRTP");

  if (check_col(pinfo->cinfo, COL_INFO))
    col_set_str(pinfo->cinfo, COL_INFO, "Context State");

  if (tree) {
    
    ti = proto_tree_add_protocol_format(tree, proto_iphc_crtp, tvb, 0, -1, 
					"%s", val_to_str(PPP_RTP_CS, ppp_vals, "Unknown"));
    
    cs_tree = proto_item_add_subtree(ti, ett_iphc_crtp);

    ti = proto_tree_add_item(cs_tree, hf_iphc_crtp_cs_flags, tvb, 0, 1, FALSE);
    ti = proto_tree_add_item(cs_tree, hf_iphc_crtp_cs_cnt, tvb, 1, 1, FALSE);

    /
    flags = tvb_get_guint8(tvb, 0);
    cnt = tvb_get_guint8(tvb, 1);

    if (flags == IPHC_CRTP_CS_CID8) {
      hf = hf_iphc_crtp_cid8;
      cid_size = 1;
      length = 3 * cnt;
    } else {
      hf = hf_iphc_crtp_cid16;
      cid_size = 2;
      length = 4 * cnt;
    }
 
    tvb_ensure_bytes_exist(tvb, offset, length);

    while (offset < length) {
      ti = proto_tree_add_item(cs_tree, hf, tvb, offset, cid_size, FALSE);
      offset += cid_size;
      ti = proto_tree_add_item(cs_tree, hf_iphc_crtp_cs_invalid, tvb, offset, 1, FALSE);
      ti = proto_tree_add_item(cs_tree, hf_iphc_crtp_seq, tvb, offset, 1, FALSE);
      ++offset;
      ti = proto_tree_add_item(cs_tree, hf_iphc_crtp_gen, tvb, offset, 1, FALSE);
      ++offset;
    }
  } /
}
