static void dissect_r3_cmd_setdatetime (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree *dt_tree = NULL;
  proto_item *dt_item = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  dt_item = proto_tree_add_text (tree, payload_tvb, 0, -1, "Set Date/Time (%02u/%02u/%02u-%u %02u:%02u:%02u)", tvb_get_guint8 (payload_tvb, 0), tvb_get_guint8 (payload_tvb, 1), tvb_get_guint8 (payload_tvb, 2), tvb_get_guint8 (payload_tvb, 3), tvb_get_guint8 (payload_tvb, 4), tvb_get_guint8 (payload_tvb, 5), tvb_get_guint8 (payload_tvb, 6));
  dt_tree = proto_item_add_subtree (dt_item, ett_r3setdatetime);

  proto_tree_add_item (dt_tree, hf_r3_setdate_year, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_month, payload_tvb, 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_day, payload_tvb, 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_dow, payload_tvb, 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_hours, payload_tvb, 4, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_minutes, payload_tvb, 5, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dt_tree, hf_r3_setdate_seconds, payload_tvb, 6, 1, ENC_LITTLE_ENDIAN);
}
