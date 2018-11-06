static void dissect_r3_cmd_definetimezone (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *starttime_item = NULL;
  proto_tree *starttime_tree = NULL;
  proto_item *endtime_item = NULL;
  proto_tree *endtime_tree = NULL;
  proto_item *daymap_item = NULL;
  proto_tree *daymap_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 i;
  guint8 tzmode;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_definetimezone_number, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);

  starttime_item = proto_tree_add_text (tree, payload_tvb, 1, 2, "Start HH:MM (%02u:%02u)", tvb_get_guint8 (payload_tvb, 1), tvb_get_guint8 (payload_tvb, 2));
  starttime_tree = proto_item_add_subtree (starttime_item, ett_r3definetimezonestarttime);
  proto_tree_add_item (starttime_tree, hf_r3_definetimezone_starttime_hours, payload_tvb, 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (starttime_tree, hf_r3_definetimezone_starttime_minutes, payload_tvb, 2, 1, ENC_LITTLE_ENDIAN);

  endtime_item = proto_tree_add_text (tree, payload_tvb, 3, 2, "End HH:MM (%02u:%02u)", tvb_get_guint8 (payload_tvb, 3), tvb_get_guint8 (payload_tvb, 4));
  endtime_tree = proto_item_add_subtree (endtime_item, ett_r3definetimezoneendtime);
  proto_tree_add_item (endtime_tree, hf_r3_definetimezone_endtime_hours, payload_tvb, 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (endtime_tree, hf_r3_definetimezone_endtime_minutes, payload_tvb, 4, 1, ENC_LITTLE_ENDIAN);

  daymap_item = proto_tree_add_text (tree, payload_tvb, 5, 1, "Day Map (0x%02x)", tvb_get_guint8 (payload_tvb, 5));
  daymap_tree = proto_item_add_subtree (daymap_item, ett_r3definetimezonedaymap);

  for (i = 0; i < 7; i++)
    proto_tree_add_boolean (daymap_tree, hf_r3_definetimezone_daymap [i], payload_tvb, 5, 1, tvb_get_guint8 (payload_tvb, 5));

  proto_tree_add_item (tree, hf_r3_definetimezone_exceptiongroup, payload_tvb, 6, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_uint (tree, hf_r3_definetimezone_mode, payload_tvb, 7, 1, (tzmode = tvb_get_guint8 (payload_tvb, 7)) & 0x0f);
  proto_tree_add_none_format (tree, hf_r3_definetimezone_calendar, payload_tvb, 7, 1, "Access Always: %s", (tzmode & 0x10) ? "True" : "False");
}
