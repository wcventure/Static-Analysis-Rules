static void dissect_r3_upstreammfgfield_adcs (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint32 i;

  for (i = 0; i < tvb_reported_length (tvb); i++)
  {
    proto_item *item = proto_tree_add_item (tree, hf_r3_adc [i], tvb, start_offset + i, 1, ENC_LITTLE_ENDIAN);
    proto_item_append_text (item, " (%.2f Volts)", (float) tvb_get_guint8 (tvb, start_offset + i) * 0.04154);
  }
}
