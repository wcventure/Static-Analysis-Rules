static int
dissect_lpp_T_almanacModel(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 1247 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *almanacModel_tvb = NULL;
  int len;

  offset = dissect_per_bit_string(tvb, offset, actx, tree, hf_index,
                                     1, 8, FALSE, &almanacModel_tvb, &len);

  if(almanacModel_tvb){
    proto_tree *subtree;

    subtree = proto_item_add_subtree(actx->created_item, ett_lpp_bitmap);
    if (len >= 1) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_1, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 2) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_2, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 3) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_3, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 4) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_4, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 5) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_5, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 6) {
      proto_tree_add_item(subtree, hf_lpp_T_almanacModel_model_6, almanacModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
  }



  return offset;
}
