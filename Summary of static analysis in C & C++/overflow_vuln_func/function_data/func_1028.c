static int
dissect_lpp_T_error_Resolution(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 307 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *error_Resolution_tvb = NULL;
  offset = dissect_per_bit_string(tvb, offset, actx, tree, -1,
                                     2, 2, FALSE, &error_Resolution_tvb, NULL);




#line 311 "../../asn1/lpp/lpp.cnf"
  if (error_Resolution_tvb) {
    actx->created_item = proto_tree_add_uint(tree, hf_index, error_Resolution_tvb, 0, 1, tvb_get_bits8(error_Resolution_tvb, 0, 2));
  }


  return offset;
}
