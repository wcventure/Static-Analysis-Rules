static void dissect_r3_upstreammfgfield_keypadchar (tvbuff_t *tvb, guint32 start_offset _U_, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item_append_text (proto_tree_add_item (tree, hf_r3_testkeypad, tvb, 0, 1, ENC_LITTLE_ENDIAN), " ('%c')", tvb_get_guint8 (tvb, 0));
}
