static int
dissect_ansi_tcap_T_parameter(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 20 "ansi_tcap.cnf"
tvbuff_t	*parameter_tvb;

  offset = dissect_ber_octet_string(TRUE, actx, tree, tvb, offset, hf_index,
                                       &parameter_tvb);
  if(!parameter_tvb)
    return offset;
  
  find_tcap_subdisector(parameter_tvb, actx, tree);



  return offset;
}
