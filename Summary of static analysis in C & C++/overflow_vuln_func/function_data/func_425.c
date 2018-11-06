static int
dissect_rtse_T_applicationProtocol(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 132 "rtse.cnf"

  offset = dissect_ber_integer(TRUE, actx, tree, tvb, offset, hf_index, &app_proto);



  return offset;
}
