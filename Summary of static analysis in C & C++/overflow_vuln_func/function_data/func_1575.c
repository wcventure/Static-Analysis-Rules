static void dissect_r3_upstreamcommand_dumpdebuglog (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *debuglog_item = proto_tree_add_text (tree, tvb, start_offset, 8, "Debug Log Record %u", tvb_get_letohs (tvb, start_offset + 0));
  proto_tree *debuglog_tree = proto_item_add_subtree (debuglog_item, ett_r3debuglogrecord);

  proto_tree_add_item (debuglog_tree, hf_r3_debuglog_recordnumber, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (debuglog_tree, hf_r3_debuglog_flags, tvb, start_offset + 2, 4, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (debuglog_tree, hf_r3_debuglog_tick, tvb, start_offset + 6, 2, ENC_LITTLE_ENDIAN);
}
