static int
dissect_lpp_ReportingDuration(tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 274 "../../asn1/lpp/lpp.cnf"
  guint32 duration;

  offset = dissect_per_constrained_integer(tvb, offset, actx, tree, hf_index,
                                                            0U, 255U, &duration, FALSE);

  proto_item_append_text(actx->created_item, " s");
  if (duration == 0) {
    proto_item_append_text(actx->created_item, " (infinite)");
  }



  return offset;
}
