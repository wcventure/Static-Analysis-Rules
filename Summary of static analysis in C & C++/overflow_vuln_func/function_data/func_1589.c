static void dissect_r3_upstreammfgfield_mortisestatelog (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint l = tvb_length_remaining (tvb, start_offset + 3);
  guint i = 0;

  proto_tree_add_item (tree, hf_r3_mortisestatelog_pointer, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_mortisestatelog_mortisetype, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_mortisestatelog_waiting, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);

  for (start_offset += 3, i = 0; i < l; i += 3)
  {
    guint state = tvb_get_guint8 (tvb, start_offset + i + 0);
    guint last = tvb_get_guint8 (tvb, start_offset + i + 1);
    guint event = tvb_get_guint8 (tvb, start_offset + i + 2);
    proto_item *ms_item = proto_tree_add_text (tree, tvb, start_offset + i, 3, "State Log Entry %2d (State=0x%02x, Last=0x%02x, Event=%s (0x%02x))", i / 3, state, last, val_to_str_ext_const (event, &r3_mortiseeventnames_ext, "[Unknown]"), event);
    proto_tree *ms_tree = proto_item_add_subtree (ms_item, ett_r3mortisestatelog);

    proto_tree_add_item (ms_tree, hf_r3_mortisestatelog_state, tvb, start_offset + i + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (ms_tree, hf_r3_mortisestatelog_last, tvb, start_offset + i + 1, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (ms_tree, hf_r3_mortisestatelog_event, tvb, start_offset + i + 2, 1, ENC_LITTLE_ENDIAN);
  }
}
