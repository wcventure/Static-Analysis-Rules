static void dissect_r3_cmd_definecalendar (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  const gchar *mn;
  guint32 i;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_definecalendar_number, payload_tvb, 0, 1, ENC_LITTLE_ENDIAN);

  for (i = 0; i < 12; i++)
  {
    guint32 daymap = tvb_get_letohl (payload_tvb, (i * 4) + 1);
    proto_item *calendar_item = proto_tree_add_text (tree, payload_tvb, (i * 4) + 1, 4, "Calendar Bit Field - %s (0x%08x)", (mn = val_to_str_ext_const (i + 1, &r3_monthnames_ext, "[Unknown Month]")), daymap);
    proto_tree *calendar_tree = proto_item_add_subtree (calendar_item, ett_r3definecalendarmonth [i + 1]);
    guint32 j;

    for (j = 0; j < 31; j++)
      proto_tree_add_none_format (calendar_tree, hf_r3_definecalendar_bits, payload_tvb, (i * 4) + 1, 4, "%s Of %s: %s", val_to_str_ext_const (j + 1, &r3_monthdaynames_ext, "[Unknown Day]"), mn, (daymap & (1 << j)) ? "Enabled" : "Disabled");
  }
}
