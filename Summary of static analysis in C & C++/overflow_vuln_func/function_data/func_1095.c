static void
dissect_glbp_hello(tvbuff_t *tvb, int offset, guint32 length _U_,
	packet_info *pinfo, proto_tree *tlv_tree)
{
  guint16 addrtype;
  guint16 addrlen;

  proto_tree_add_item(tlv_tree, hf_glbp_hello_unknown10, tvb, offset, 1,  FALSE);
  offset ++;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_vgstate, tvb, offset, 1,  FALSE);
  offset ++;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_unknown11, tvb, offset, 1,  FALSE);
  offset ++;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_priority, tvb, offset, 1,  FALSE);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_unknown12, tvb, offset, 2, FALSE);
  offset += 2;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_helloint, tvb, offset, 4, FALSE);
  offset += 4;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_holdint, tvb, offset, 4, FALSE);
  offset += 4;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_redirect, tvb, offset, 2, FALSE);
  offset += 2;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_timeout, tvb, offset, 2, FALSE);
  offset += 2;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_unknown13, tvb, offset, 2, FALSE);
  offset += 2;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_addrtype, tvb, offset, 1, FALSE);
  addrtype = tvb_get_guint8(tvb, offset);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_hello_addrlen, tvb, offset, 1, FALSE);
  addrlen = tvb_get_guint8(tvb, offset);
  offset++;
  switch (addrtype) {
    case 1:
    proto_tree_add_item(tlv_tree, hf_glbp_hello_virtualipv4, tvb, offset, addrlen, FALSE);
    break;
  case 2:
    proto_tree_add_item(tlv_tree, hf_glbp_hello_virtualipv6, tvb, offset, addrlen, FALSE);
    break;
  default:
    proto_tree_add_item(tlv_tree, hf_glbp_hello_virtualunk, tvb, offset, addrlen, FALSE);
    break;
  }
  offset += addrlen;

  col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
    val_to_str(addrtype, glbp_addr_type_vals, "%d"));
}
