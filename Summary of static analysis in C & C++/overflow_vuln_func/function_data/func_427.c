static int
dissect_rtse_T_single_ASN1_type(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 128 "rtse.cnf"
  offset=call_rtse_oid_callback(object_identifier_id, tvb, offset, actx->pinfo, top_tree);




  return offset;
}
