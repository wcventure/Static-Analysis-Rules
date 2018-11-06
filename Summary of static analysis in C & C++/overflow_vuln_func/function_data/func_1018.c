static int
dissect_lpp_T_utcDeltaTlsf_01(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
  offset = dissect_per_constrained_integer(tvb, offset, actx, tree, hf_index,
                                                            -128, 127U, NULL, FALSE);

#line 1118 "../../asn1/lpp/lpp.cnf"
  proto_item_append_text(actx->created_item, " s");


  return offset;
}
