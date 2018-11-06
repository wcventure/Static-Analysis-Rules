static void dissect_r3_upstreamcommand_dumpnvram (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  tvb_ensure_bytes_exist (tvb, start_offset, 3);

  proto_tree_add_item (tree, hf_r3_nvramdump_record, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_nvramdump_length, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_nvramdump_data, tvb, start_offset + 3, tvb_get_guint8 (tvb, start_offset + 2), ENC_NA);
}
