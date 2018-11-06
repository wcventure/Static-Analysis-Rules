static void dissect_r3_cmd_dpac (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  /
  proto_tree_add_item (tree, hf_r3_dpac_action, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_dpac_waittime, payload_tvb, 1, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_dpac_command, payload_tvb, 3, -1, ENC_NA);
}
