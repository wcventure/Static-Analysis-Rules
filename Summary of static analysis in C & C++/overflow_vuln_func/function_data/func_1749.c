static int
dissect_gsm_map_T_extType(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 446 "gsmmap.cnf"

  proto_tree_add_text(tree, tvb, offset, -1, "Extension Data");
  call_dissector(data_handle, tvb, pinfo, tree);	
  offset = tvb_length_remaining(tvb,offset);		



  return offset;
}
