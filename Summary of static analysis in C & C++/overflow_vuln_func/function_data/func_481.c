static void
dissect_iphc_crtp_cudp16(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree *cudp_tree;
  proto_item *ti = NULL;
  guint length;
  guint hdr_length;
  int offset = 0;

  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo,COL_PROTOCOL, "CRTP");

  if (check_col(pinfo->cinfo, COL_INFO))
    col_set_str(pinfo->cinfo, COL_INFO, "Compressed UDP 16");

  length = tvb_reported_length(tvb);

  if (tree) {
    ti = proto_tree_add_protocol_format(tree, proto_iphc_crtp, tvb, 0, -1, 
					"%s", val_to_str(PPP_RTP_CUDP16, ppp_vals, "Unknown"));
    cudp_tree = proto_item_add_subtree(ti, ett_iphc_crtp);

    hdr_length = 3;

    ti = proto_tree_add_item(cudp_tree, hf_iphc_crtp_cid16, tvb, 0, 2, FALSE);
    ti = proto_tree_add_item(cudp_tree, hf_iphc_crtp_seq, tvb, 2, 1, FALSE);

    offset += hdr_length;
    length -= hdr_length;

    ti = proto_tree_add_text(cudp_tree, tvb, offset, length, "Data (%d bytes)", length);
  } /
}
