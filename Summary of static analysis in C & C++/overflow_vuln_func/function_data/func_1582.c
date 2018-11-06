static void dissect_r3_upstreammfgfield_timerchain (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint l = tvb_length_remaining (tvb, start_offset + 3);
  guint i;

  proto_tree_add_item (tree, hf_r3_timerchain_newtick, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_timerchain_currentboundary, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);

  for (start_offset += 3, i = 0; i < l; i += 12)
  {
    proto_item *tc_item = proto_tree_add_text (tree, tvb, start_offset + i, 12, "Timer Chain Entry");
    proto_tree *tc_tree = proto_item_add_subtree (tc_item, ett_r3timerchain);

    proto_tree_add_item (tc_tree, hf_r3_timerchain_tasktag, tvb, start_offset + i + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (tc_tree, hf_r3_timerchain_address, tvb, start_offset + i + 1, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (tc_tree, hf_r3_timerchain_reload, tvb, start_offset + i + 3, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (tc_tree, hf_r3_timerchain_boundary, tvb, start_offset + i + 5, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (tc_tree, hf_r3_timerchain_count, tvb, start_offset + i + 6, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (tc_tree, hf_r3_timerchain_flags, tvb, start_offset + i + 8, 4, ENC_LITTLE_ENDIAN);
  }
}
