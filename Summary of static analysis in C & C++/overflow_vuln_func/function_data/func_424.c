static int
dissect_rtse_SessionConnectionIdentifier(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 136 "rtse.cnf"
  if(open_request && check_col(actx->pinfo->cinfo, COL_INFO))
    col_append_fstr(actx->pinfo->cinfo, COL_INFO, "Recover");
    offset = dissect_ber_old_sequence(implicit_tag, actx, tree, tvb, offset,
                                       SessionConnectionIdentifier_sequence, hf_index, ett_rtse_SessionConnectionIdentifier);




  return offset;
}
