static void dissect_r3_cmd_filters (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *filter_item = NULL;
  proto_tree *filter_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 i;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_filter_type, payload_tvb, 1, 1, ENC_LITTLE_ENDIAN);

  filter_item = proto_tree_add_text (tree, payload_tvb, 2, -1, "Filters (%u specified)", tvb_get_guint8 (payload_tvb, 0));
  filter_tree = proto_item_add_subtree (filter_item, ett_r3filters);

  for (i = 0; i < tvb_get_guint8 (payload_tvb, 0); i++)
    proto_tree_add_item (filter_tree, hf_r3_filter_list, payload_tvb, i + 2, 1, ENC_LITTLE_ENDIAN);
}
