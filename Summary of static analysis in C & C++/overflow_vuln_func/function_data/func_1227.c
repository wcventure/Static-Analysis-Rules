static int
dissect_ansi_tcap_UserAbortInformation(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
  offset = dissect_ber_tagged_type(implicit_tag, actx, tree, tvb, offset,
                                      hf_index, BER_CLASS_PRI, 24, FALSE, dissect_ansi_tcap_EXTERNAL);

  return offset;
}