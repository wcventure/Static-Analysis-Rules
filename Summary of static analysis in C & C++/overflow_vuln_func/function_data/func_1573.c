static void dissect_r3_upstreamcommand_dumpdeclinedlog (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *declinedlog_item = NULL;
  proto_tree *declinedlog_tree = NULL;
  guint8 cred1type = 0;
  guint8 cred2type = 0;

  declinedlog_item = proto_tree_add_text (tree, tvb, start_offset, 49, "Declined Log Record %u", tvb_get_letohs (tvb, start_offset + 0));
  declinedlog_tree = proto_item_add_subtree (declinedlog_item, ett_r3declinedlogrecord);

  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_recordnumber, tvb, start_offset, 2, ENC_LITTLE_ENDIAN); start_offset += 2;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_year, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);        start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_month, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);       start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_day, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);         start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_hour, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);        start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_minute, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);      start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_second, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);      start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_usernumber, tvb, start_offset, 2, ENC_LITTLE_ENDIAN);   start_offset += 2;

  cred1type = tvb_get_guint8 (tvb, start_offset) & 0x07;
  cred2type = (tvb_get_guint8 (tvb, start_offset) & 0x38) >> 3;

  proto_tree_add_uint (declinedlog_tree, hf_r3_declinedlog_cred1type, tvb, start_offset, 1, cred1type);   start_offset += 0;
  proto_tree_add_uint (declinedlog_tree, hf_r3_declinedlog_cred2type, tvb, start_offset, 1, cred2type);   start_offset += 1;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_cred1, tvb, start_offset, 19, ENC_NA);          start_offset += 19;
  proto_tree_add_item (declinedlog_tree, hf_r3_declinedlog_cred2, tvb, start_offset, 19, ENC_NA);
}
