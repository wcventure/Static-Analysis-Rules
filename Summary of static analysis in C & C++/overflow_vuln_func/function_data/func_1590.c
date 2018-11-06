static void dissect_r3_upstreammfgfield_mortisepins (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *iopins_item = proto_tree_add_text (tree, tvb, start_offset, 1, "Mortise Pin States (0x%02x)", tvb_get_guint8 (tvb, start_offset));
  proto_tree *iopins_tree = proto_item_add_subtree (iopins_item, ett_r3iopins);

  proto_tree_add_boolean (iopins_tree, hf_r3_mortisepins_s1, tvb, start_offset, 1, tvb_get_guint8 (tvb, start_offset));
  proto_tree_add_boolean (iopins_tree, hf_r3_mortisepins_s2, tvb, start_offset, 1, tvb_get_guint8 (tvb, start_offset));
  proto_tree_add_boolean (iopins_tree, hf_r3_mortisepins_s3, tvb, start_offset, 1, tvb_get_guint8 (tvb, start_offset));
  proto_tree_add_boolean (iopins_tree, hf_r3_mortisepins_s4, tvb, start_offset, 1, tvb_get_guint8 (tvb, start_offset));
}
