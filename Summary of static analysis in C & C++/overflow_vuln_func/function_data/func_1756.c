static int
dissect_gsm_map_ForwardingOptions(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       NULL);

#line 453 "gsmmap.cnf"

	proto_tree_add_item(tree, hf_gsm_map_notification_to_forwarding_party, tvb, 0,1,FALSE);
	proto_tree_add_item(tree, hf_gsm_map_redirecting_presentation, tvb, 0,1,FALSE);
	proto_tree_add_item(tree, hf_gsm_map_notification_to_calling_party, tvb, 0,1,FALSE);
	proto_tree_add_item(tree, hf_gsm_map_forwarding_reason, tvb, 0,1,FALSE);

  return offset;
}
