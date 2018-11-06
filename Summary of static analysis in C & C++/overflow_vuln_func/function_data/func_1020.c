static int
dissect_lpp_T_reportingInterval(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 264 "../../asn1/lpp/lpp.cnf"
  guint32 idx;
  const gchar *interval[10] = {"", ": 1 s", ": 2 s", ": 4 s", ": 8 s", ": 10 s",
                               ": 16 s", ": 20 s", ": 32 s", ": 64 s"};

  offset = dissect_per_enumerated(tvb, offset, actx, tree, hf_index,
                                     10, &idx, FALSE, 0, NULL);




#line 271 "../../asn1/lpp/lpp.cnf"
  proto_item_append_text(actx->created_item, "%s", interval[idx]);


  return offset;
}
