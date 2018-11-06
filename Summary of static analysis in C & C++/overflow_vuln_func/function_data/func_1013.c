static int
dissect_lpp_T_kp(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 1085 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *kp_tvb = NULL;
  offset = dissect_per_bit_string(tvb, offset, actx, tree, -1,
                                     2, 2, FALSE, &kp_tvb, NULL);




#line 1089 "../../asn1/lpp/lpp.cnf"
  if (kp_tvb) {
    actx->created_item = proto_tree_add_uint(tree, hf_index, kp_tvb, 0, 1, tvb_get_bits8(kp_tvb, 0, 2));
  }


  return offset;
}
