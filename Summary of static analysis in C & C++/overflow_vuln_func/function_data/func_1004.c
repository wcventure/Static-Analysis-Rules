static int
dissect_lpp_T_gnss_Utc_WNlsf(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
  offset = dissect_per_constrained_integer(tvb, offset, actx, tree, hf_index,
                                                            0U, 255U, NULL, FALSE);

#line 1037 "../../asn1/lpp/lpp.cnf"
  proto_item_append_text(actx->created_item, " weeks");


  return offset;
}
