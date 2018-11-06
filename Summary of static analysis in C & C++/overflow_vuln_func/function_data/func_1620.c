static void dissect_r3_cmd_clearnvram (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *nvram_item = NULL;
  proto_tree *nvram_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 nvramclearoptions = tvb_get_letohs (payload_tvb, 0);
  guint32 i;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  nvram_item = proto_tree_add_text (tree, payload_tvb, 0, 2, "NVRAM Clean Options (0x%04x)", nvramclearoptions);
  nvram_tree = proto_item_add_subtree (nvram_item, ett_r3clearnvram);

  for (i = 0; i < 16; i++)
    proto_tree_add_boolean (nvram_tree, hf_r3_nvramclearoptions [i], payload_tvb, 0, 2, nvramclearoptions);
}
