static int
dissect_ansi_tcap_Invoke(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 30 "ansi_tcap.cnf"
  ansi_tcap_private.d.pdu = 1;



  offset = dissect_ber_sequence(implicit_tag, actx, tree, tvb, offset,
                                   Invoke_sequence, hf_index, ett_ansi_tcap_Invoke);

  return offset;
}
