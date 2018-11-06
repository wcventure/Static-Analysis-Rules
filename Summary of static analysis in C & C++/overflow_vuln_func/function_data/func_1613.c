static void dissect_r3_cmd_alarmconfigure (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *alarm_item = NULL;
  proto_tree *alarm_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 offset = 0;
  guint32 alarms = 0;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  alarm_item = proto_tree_add_text (tree, payload_tvb, 0, -1, "Alarm List (0 items)");
  alarm_tree = proto_item_add_subtree (alarm_item, ett_r3alarmlist);

  while (offset < tvb_reported_length (payload_tvb))
  {
    proto_item *alarmcfg_item = NULL;
    proto_tree *alarmcfg_tree = NULL;
    const gchar *ai;
    const gchar *as;

    if (!(ai = match_strval_ext (tvb_get_guint8 (payload_tvb, offset + 1), &r3_alarmidnames_ext)))
    {
      ai = "[Unknown Alarm ID]";
      as = "N/A";
    }
    else
      as = (tvb_get_guint8 (payload_tvb, offset + 2) & 0xfe) ? "Error" : (tvb_get_guint8 (payload_tvb, offset + 2) & 0x01) ? "Enabled" : "Disabled";

    alarmcfg_item = proto_tree_add_text (alarm_tree, payload_tvb, offset, tvb_get_guint8 (payload_tvb, offset), "Alarm Item (%s, %s)", ai, as);
    alarmcfg_tree = proto_item_add_subtree (alarmcfg_item, ett_r3alarmcfg);

    proto_tree_add_item (alarmcfg_tree, hf_r3_alarm_length, payload_tvb, offset + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (alarmcfg_tree, hf_r3_alarm_id, payload_tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (alarmcfg_tree, hf_r3_alarm_state, payload_tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);

    alarms++;
    offset += tvb_get_guint8 (payload_tvb, offset);
  }

  if (alarms)
    proto_item_set_text (alarm_item, "Alarm List (%d items)", alarms);
}
