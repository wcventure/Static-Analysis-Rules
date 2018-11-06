static void dissect_r3_upstreammfgfield_nvramchecksumvalue (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_nvramchecksumvalue, tvb, start_offset + 0, 4, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_nvramchecksumvalue_fixup, tvb, start_offset + 4, 4, ENC_LITTLE_ENDIAN);
}
