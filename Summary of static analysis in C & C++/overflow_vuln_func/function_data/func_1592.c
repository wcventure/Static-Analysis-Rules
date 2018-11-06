static void dissect_r3_upstreammfgfield_magcard (tvbuff_t *tvb, guint32 start_offset _U_, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_testmagcard, tvb, 0, -1, ENC_ASCII|ENC_NA);
}
