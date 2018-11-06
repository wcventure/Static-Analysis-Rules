static int
dissect_rtse_T_indirect_reference(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 115 "rtse.cnf"
  char *oid;

  offset = dissect_ber_integer(FALSE, actx, tree, tvb, offset,
                hf_rtse_indirect_reference,
                &indir_ref);

  /
  if((oid = find_oid_by_pres_ctx_id(actx->pinfo, indir_ref)) != NULL) {
    g_snprintf(object_identifier_id, MAX_OID_STR_LEN, "%s", oid);
  }
	



  return offset;
}
