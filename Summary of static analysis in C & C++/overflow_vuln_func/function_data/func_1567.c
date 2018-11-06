static void dissect_r3_upstreamcommand_dumpeventlog (tvbuff_t *tvb, guint32 start_offset, guint32 length, packet_info *pinfo, proto_tree *tree)
{
  proto_item *eventlog_item = NULL;
  proto_tree *eventlog_tree = NULL;
  const gchar *en = NULL;

  tvb_ensure_bytes_exist (tvb, start_offset, 11);

  if (length != 11)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Malformed event log record -- expected 10 octets");
  else
  {
    en = val_to_str_ext_const (tvb_get_guint8 (tvb, start_offset + 10), &r3_eventnames_ext, "[Unknown Event]");

    eventlog_item = proto_tree_add_text (tree, tvb, start_offset, 10, "Event Log Record %u (%s)", tvb_get_letohs (tvb, start_offset + 0), en);
    eventlog_tree = proto_item_add_subtree (eventlog_item, ett_r3eventlogrecord);

    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_recordnumber, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_year, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_month, tvb, start_offset + 3, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_day, tvb, start_offset + 4, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_hour, tvb, start_offset + 5, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_minute, tvb, start_offset + 6, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_second, tvb, start_offset + 7, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_usernumber, tvb, start_offset + 8, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (eventlog_tree, hf_r3_eventlog_event, tvb, start_offset + 10, 1, ENC_LITTLE_ENDIAN);
  }
}
