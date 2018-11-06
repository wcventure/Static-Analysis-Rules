static int
dissect_ansi_tcap_OperationCode(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
  offset = dissect_ber_choice(actx, tree, tvb, offset,
                                 OperationCode_choice, hf_index, ett_ansi_tcap_OperationCode,
                                 &ansi_tcap_private.d.OperationCode);

#line 12 "ansi_tcap.cnf"
  ansi_tcap_private.d.OperationCode_item = actx->created_item;

  return offset;
}
