static void dissect_r3_cmdmfg_rebuildlrucache (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_commandmfglength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_commandmfg, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
}
