static int
dissect_lpp_T_orbitModel(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 1221 "../../asn1/lpp/lpp.cnf"
  tvbuff_t *orbitModel_tvb = NULL;
  int len;

  offset = dissect_per_bit_string(tvb, offset, actx, tree, hf_index,
                                     1, 8, FALSE, &orbitModel_tvb, &len);

  if(orbitModel_tvb){
    proto_tree *subtree;

    subtree = proto_item_add_subtree(actx->created_item, ett_lpp_bitmap);
    if (len >= 1) {
      proto_tree_add_item(subtree, hf_lpp_T_orbitModel_model_1, orbitModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 2) {
      proto_tree_add_item(subtree, hf_lpp_T_orbitModel_model_2, orbitModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 3) {
      proto_tree_add_item(subtree, hf_lpp_T_orbitModel_model_3, orbitModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 4) {
      proto_tree_add_item(subtree, hf_lpp_T_orbitModel_model_4, orbitModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
    if (len >= 5) {
      proto_tree_add_item(subtree, hf_lpp_T_orbitModel_model_5, orbitModel_tvb, 0, 1, ENC_BIG_ENDIAN);
    }
  }



  return offset;
}
