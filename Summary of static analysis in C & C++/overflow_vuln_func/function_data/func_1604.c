static void dissect_r3_cmd_getconfig (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *config_item = NULL;
  proto_tree *config_tree = NULL;
  guint32 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  guint32 i;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  config_item = proto_tree_add_item (tree, hf_r3_configitems, tvb, start_offset + 2, cmdLen - 2, ENC_NA);
  config_tree = proto_item_add_subtree (config_item, ett_r3configitem);

  for (i = 2; i < cmdLen; i++)
    proto_tree_add_item (config_tree, hf_r3_configitem, tvb, start_offset + i, 1, ENC_LITTLE_ENDIAN);
}
