static int
dissect_ansi_tcap_PackageType(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
  offset = dissect_ber_choice(actx, tree, tvb, offset,
                                 PackageType_choice, hf_index, ett_ansi_tcap_PackageType,
                                 NULL);

  return offset;
}
