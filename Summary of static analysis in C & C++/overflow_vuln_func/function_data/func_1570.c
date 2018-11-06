static void dissect_r3_upstreamcommand_dpac (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  /
  proto_tree_add_item (tree, hf_r3_dpacreply_stuff, tvb, start_offset + 2, 1, ENC_NA);
  proto_tree_add_item (tree, hf_r3_dpacreply_length, tvb, start_offset + 3, 1, ENC_NA);
  proto_tree_add_item (tree, hf_r3_dpacreply_reply, tvb, start_offset + 4, -1, ENC_NA);
}
