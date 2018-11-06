static int
dissect_ansi_tcap_ObjectIDApplicationContext(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 72 "ansi_tcap.cnf"

 static const char * oid_str;

   offset = dissect_ber_tagged_type(implicit_tag, actx, tree, tvb, offset,
                                      hf_index, BER_CLASS_PRI, 28, TRUE, dissect_ansi_tcap_OBJECT_IDENTIFIER);

 	ansi_tcap_private.objectApplicationId_oid= (void*) oid_str;
	ansi_tcap_private.oid_is_present=TRUE;



  return offset;
}
