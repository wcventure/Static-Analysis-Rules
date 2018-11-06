static int
dissect_lpp_T_error_NumSamples(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 331 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *error_NumSamples_tvb = NULL;
  offset = dissect_per_bit_string(tvb, offset, actx, tree, -1,
                                     3, 3, FALSE, &error_NumSamples_tvb, NULL);




#line 335 "../../asn1/lpp/lpp.cnf"
  if (error_NumSamples_tvb) {
    actx->created_item = proto_tree_add_uint(tree, hf_index, error_NumSamples_tvb, 0, 1, tvb_get_bits8(error_NumSamples_tvb, 0, 3));
  }


  return offset;
}
