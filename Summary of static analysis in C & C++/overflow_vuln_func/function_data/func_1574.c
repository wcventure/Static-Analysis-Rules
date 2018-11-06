static void dissect_r3_upstreamcommand_dumpalarmlog (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *alarmlog_item = proto_tree_add_text (tree, tvb, start_offset, 9, "Alarm Log Record %u", tvb_get_letohs (tvb, start_offset + 0));
  proto_tree *alarmlog_tree = proto_item_add_subtree (alarmlog_item, ett_r3alarmlogrecord);

  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_recordnumber, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_year, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_month, tvb, start_offset + 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_day, tvb, start_offset + 4, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_hour, tvb, start_offset + 5, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_minute, tvb, start_offset + 6, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_second, tvb, start_offset + 7, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_id, tvb, start_offset + 8, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (alarmlog_tree, hf_r3_alarmlog_usernumber, tvb, start_offset + 9, 2, ENC_LITTLE_ENDIAN);
}
