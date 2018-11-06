static void dissect_r3_cmd_extendedresponse (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
}
