static void
dissect_glbp_reqresp(tvbuff_t *tvb, int offset, guint32 length _U_,
	packet_info *pinfo _U_, proto_tree *tlv_tree)
{
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_forwarder, tvb, offset, 1, FALSE);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_vfstate, tvb, offset, 1, FALSE);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_unknown21, tvb, offset, 1, FALSE);
  offset += 1;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_priority, tvb, offset, 1, FALSE);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_weight, tvb, offset, 1, FALSE);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_unknown22, tvb, offset, 7, FALSE);
  offset += 7;
  proto_tree_add_item(tlv_tree, hf_glbp_reqresp_virtualmac, tvb, offset, 6, FALSE);
  offset += 6 ;
}
