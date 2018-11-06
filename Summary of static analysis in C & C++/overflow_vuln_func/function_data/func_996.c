static int
dissect_lpp_T_gloP1(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 781 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *gloP1_tvb = NULL;
  offset = dissect_per_bit_string(tvb, offset, actx, tree, -1,
                                     2, 2, FALSE, &gloP1_tvb, NULL);




#line 785 "../../asn1/lpp/lpp.cnf"
  if (gloP1_tvb) {
    actx->created_item = proto_tree_add_uint(tree, hf_index, gloP1_tvb, 0, 1, tvb_get_bits8(gloP1_tvb, 0, 2));
    proto_item_append_text(actx->created_item, " mn");
  }


  return offset;
}
