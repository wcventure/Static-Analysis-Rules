static void dissect_r3_cmd_eventlogdump (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *starttime_item = NULL;
  proto_tree *starttime_tree = NULL;
  proto_item *endtime_item = NULL;
  proto_tree *endtime_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  starttime_item = proto_tree_add_text (tree, payload_tvb, 0, 5, "Start YY/MM/DD HH:MM (%02u/%02u/%02u %02u:%02u)", tvb_get_guint8 (payload_tvb, 0), tvb_get_guint8 (payload_tvb, 1), tvb_get_guint8 (payload_tvb, 2), tvb_get_guint8 (payload_tvb, 3), tvb_get_guint8 (payload_tvb, 4));
  starttime_tree = proto_item_add_subtree (starttime_item, ett_r3eventlogdumpstarttime);
  proto_tree_add_item (starttime_tree, hf_r3_eventlogdump_starttime_year, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (starttime_tree, hf_r3_eventlogdump_starttime_month, payload_tvb, 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (starttime_tree, hf_r3_eventlogdump_starttime_day, payload_tvb, 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (starttime_tree, hf_r3_eventlogdump_starttime_hours, payload_tvb, 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (starttime_tree, hf_r3_eventlogdump_starttime_minutes, payload_tvb, 4, 1, ENC_LITTLE_ENDIAN);

  endtime_item = proto_tree_add_text (tree, payload_tvb, 5, 5, "End YY/MM/DD HH:MM (%02u/%02u/%02u %02u:%02u)", tvb_get_guint8 (payload_tvb, 5), tvb_get_guint8 (payload_tvb, 6), tvb_get_guint8 (payload_tvb, 7), tvb_get_guint8 (payload_tvb, 8), tvb_get_guint8 (payload_tvb, 9));
  endtime_tree = proto_item_add_subtree (endtime_item, ett_r3eventlogdumpendtime);
  proto_tree_add_item (endtime_tree, hf_r3_eventlogdump_endtime_year, payload_tvb, 5, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (endtime_tree, hf_r3_eventlogdump_endtime_month, payload_tvb, 6, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (endtime_tree, hf_r3_eventlogdump_endtime_day, payload_tvb, 7, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (endtime_tree, hf_r3_eventlogdump_endtime_hours, payload_tvb, 8, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (endtime_tree, hf_r3_eventlogdump_endtime_minutes, payload_tvb, 9, 1, ENC_LITTLE_ENDIAN);

  proto_tree_add_item (tree, hf_r3_eventlogdump_user, payload_tvb, 10, 2, ENC_LITTLE_ENDIAN);
}
