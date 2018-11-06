static void dissect_r3_upstreammfgfield_hardwareid (tvbuff_t *tvb, guint32 start_offset _U_, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_hardwareid_board, tvb, 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_hardwareid_cpuid, tvb, 1, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_hardwareid_cpurev, tvb, 3, 1, ENC_LITTLE_ENDIAN);
}
