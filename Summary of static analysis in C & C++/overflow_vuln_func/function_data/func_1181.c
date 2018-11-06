static gint dissect_ipm_identifier (tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree,
                                    gint offset, gboolean subject)
{
  proto_tree *field_tree;
  proto_item *tf, *hidden_item;
  gchar      *ipm_id;
  gint        length, modifier, ipm_id_length;

  length = tvb_get_guint8 (tvb, offset);
  modifier = (length & 0xC0) >> 6;
  ipm_id_length = length & 0x3F;

  tf = proto_tree_add_uint_format (tree, hf_envelope_ipm_id_length,
                                   tvb, offset, 1, ipm_id_length,
                                   "IPM Identifier Length: %u",
                                   ipm_id_length);
  field_tree = proto_item_add_subtree (tf, ett_envelope_ipm_id_length);
  if ((dmp.msg_id_type == NAT_MSG_ID || modifier != IPM_MODIFIER_X400) && dmp_nat_decode == NAT_DECODE_THALES) {
    proto_tree_add_item (field_tree, hf_thales_ipm_id_modifier, tvb, offset, 1, ENC_BIG_ENDIAN);
  } else {
    proto_tree_add_item (field_tree, hf_envelope_ipm_id_modifier, tvb, offset, 1, ENC_BIG_ENDIAN);
  }
  proto_tree_add_item (field_tree, hf_envelope_ipm_id_length, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;
  
  if (modifier == IPM_MODIFIER_X400 || dmp_nat_decode == NAT_DECODE_DMP) {
    ipm_id = dissect_7bit_string (tvb, offset, ipm_id_length);
  } else if (dmp_nat_decode == NAT_DECODE_THALES) {
    ipm_id = dissect_thales_ipm_id (tvb, offset, ipm_id_length, modifier);
  } else {
    ipm_id = tvb_bytes_to_str (tvb, offset, ipm_id_length);
  }
  proto_item_append_text (tf, " (%zu bytes decompressed)", strlen (ipm_id));
  ipm_id = format_text (ipm_id, strlen (ipm_id));
  if (subject) {
    proto_tree_add_string (tree, hf_message_subj_ipm_id, tvb, offset, ipm_id_length, ipm_id);
    hidden_item = proto_tree_add_string (tree, hf_ipm_id, tvb, offset, ipm_id_length, ipm_id);
    /
    dmp.subj_id = GPOINTER_TO_UINT (g_hash_table_lookup (dmp_long_id_hash_table, ipm_id));
  } else {
    proto_tree_add_string (tree, hf_envelope_ipm_id, tvb, offset, ipm_id_length, ipm_id);
    hidden_item = proto_tree_add_string (tree, hf_ipm_id, tvb, offset, ipm_id_length, ipm_id);
    /
    g_hash_table_insert (dmp_long_id_hash_table, g_strdup (ipm_id), GUINT_TO_POINTER ((guint)dmp.msg_id));
  }
  PROTO_ITEM_SET_HIDDEN (hidden_item);
  offset += ipm_id_length;

  return offset;
}
