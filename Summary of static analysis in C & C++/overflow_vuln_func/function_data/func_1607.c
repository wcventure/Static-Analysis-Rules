static void dissect_r3_cmd_defineexception (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *startdate_item = NULL;
  proto_tree *startdate_tree = NULL;
  proto_item *enddate_item = NULL;
  proto_tree *enddate_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_defineexception_number, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);

  startdate_item = proto_tree_add_text (tree, payload_tvb, 1, 4, "Start MM/DD HH:MM (%02u/%02u %02u:%02u)", tvb_get_guint8 (payload_tvb, 1), tvb_get_guint8 (payload_tvb, 2), tvb_get_guint8 (payload_tvb, 3), tvb_get_guint8 (payload_tvb, 4));
  startdate_tree = proto_item_add_subtree (startdate_item, ett_r3defineexceptionstartdate);
  proto_tree_add_item (startdate_tree, hf_r3_defineexception_startdate_month, payload_tvb, 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (startdate_tree, hf_r3_defineexception_startdate_day, payload_tvb, 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (startdate_tree, hf_r3_defineexception_startdate_hours, payload_tvb, 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (startdate_tree, hf_r3_defineexception_startdate_minutes, payload_tvb, 4, 1, ENC_LITTLE_ENDIAN);

  enddate_item = proto_tree_add_text (tree, payload_tvb, 5, 4, "End MM/DD HH:MM (%02u/%02u %02u:%02u)", tvb_get_guint8 (payload_tvb, 5), tvb_get_guint8 (payload_tvb, 6), tvb_get_guint8 (payload_tvb, 7), tvb_get_guint8 (payload_tvb, 8));
  enddate_tree = proto_item_add_subtree (enddate_item, ett_r3defineexceptionenddate);
  proto_tree_add_item (enddate_tree, hf_r3_defineexception_enddate_month, payload_tvb, 5, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (enddate_tree, hf_r3_defineexception_enddate_day, payload_tvb, 6, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (enddate_tree, hf_r3_defineexception_enddate_hours, payload_tvb, 7, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (enddate_tree, hf_r3_defineexception_enddate_minutes, payload_tvb, 8, 1, ENC_LITTLE_ENDIAN);
}
