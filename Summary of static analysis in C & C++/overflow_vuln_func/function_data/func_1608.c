static void dissect_r3_cmd_defineexceptiongroup (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *bits_item = NULL;
  proto_tree *bits_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 i = 0;
  guint32 bit = 0;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_defineexceptiongroup_number, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);

  bits_item = proto_tree_add_text (tree, payload_tvb, 1, -1, "Exception Group Bit Field");
  bits_tree = proto_item_add_subtree (bits_item, ett_r3defineexceptiongroupbits);

  for (i = 1; i < tvb_reported_length (payload_tvb); i++)
  {
    guint32 j;
    guint8 byte = tvb_get_guint8 (payload_tvb, i);

    for (j = 0; j < 8; j++)
      proto_tree_add_none_format (bits_tree, hf_r3_defineexceptiongroup_bits, payload_tvb, i, 1, "Exception Group %2d: %s", bit++, (byte & (1 << j)) ? "Enabled" : "Disabled");
  }
}
