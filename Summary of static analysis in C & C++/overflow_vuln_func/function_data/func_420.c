guint32
dissect_per_object_identifier(tvbuff_t *tvb, guint32 offset, asn1_ctx_t *actx _U_, proto_tree *tree, int hf_index, tvbuff_t **value_tvb)
{
  guint length;
  char *str;
  tvbuff_t *val_tvb = NULL;
  proto_item *item = NULL;
  header_field_info *hfi;

DEBUG_ENTRY("dissect_per_object_identifier");

  offset = dissect_per_length_determinant(tvb, offset, actx, tree, hf_per_object_identifier_length, &length);
  if (actx->aligned) BYTE_ALIGN_OFFSET(offset);
  val_tvb = new_octet_aligned_subset(tvb, offset, length);
	
  hfi = proto_registrar_get_nth(hf_index);
  if (hfi->type == FT_OID) {
    item = proto_tree_add_item(tree, hf_index, val_tvb, 0, length, FALSE);
  } else if (IS_FT_STRING(hfi->type)) {
    str = oid_to_str(tvb_get_ptr(val_tvb, 0, length), length);
    item = proto_tree_add_string(tree, hf_index, val_tvb, 0, length, str);
  } else {
    DISSECTOR_ASSERT_NOT_REACHED();
  }

  if (value_tvb) *value_tvb = val_tvb;

  offset += 8 * length;

  return offset;
}
