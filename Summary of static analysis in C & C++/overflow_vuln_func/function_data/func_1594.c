static void dissect_r3_response_singlebyte (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_responsetype, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_responsetocommand, tvb, start_offset + 3, 1, ENC_LITTLE_ENDIAN);
}
