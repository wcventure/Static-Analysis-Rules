static int
dissect_lpp_T_cnavE(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 698 "../../asn1/lpp/lpp.cnf"
  guint64 cnavE;
  int curr_offset = offset;
  offset = dissect_per_constrained_integer_64b(tvb, offset, actx, tree, hf_index,
                                                            0U, G_GUINT64_CONSTANT(8589934591), &cnavE, FALSE);




#line 703 "../../asn1/lpp/lpp.cnf"
  PROTO_ITEM_SET_HIDDEN(actx->created_item);
  actx->created_item = proto_tree_add_uint64_format_value(tree, hf_index, tvb, curr_offset>>3, (offset+7-curr_offset)>>3,
                                                          cnavE, "%e (%"G_GINT64_MODIFIER"u)",
                                                          (double)cnavE*pow(2, -34), cnavE);


  return offset;
}
