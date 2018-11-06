static int
dissect_rtse_CommonReference(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 151 "rtse.cnf"
  tvbuff_t *string = NULL;
    offset = dissect_ber_restricted_string(implicit_tag, BER_UNI_TAG_UTCTime,
                                            actx, tree, tvb, offset, hf_index,
                                            &string);

  if(open_request && string && check_col(actx->pinfo->cinfo, COL_INFO))
    col_append_fstr(actx->pinfo->cinfo, COL_INFO, " %s", tvb_format_text(string, 0, tvb_length(string)));



  return offset;
}
