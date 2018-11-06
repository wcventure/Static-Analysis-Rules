static int
dissect_lpp_T_dataID(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 381 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *dataID_tvb = NULL;
  offset = dissect_per_bit_string(tvb, offset, actx, tree, -1,
                                     2, 2, FALSE, &dataID_tvb, NULL);




#line 385 "../../asn1/lpp/lpp.cnf"
  if (dataID_tvb) {
    actx->created_item = proto_tree_add_uint(tree, hf_index, dataID_tvb, 0, 1, tvb_get_bits8(dataID_tvb, 0, 2));
  }


  return offset;
}
