static void dissect_r3_upstreamcommand_dumpnvramrle (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_tree_add_item (tree, hf_r3_nvramdumprle_record, tvb, start_offset + 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_nvramdumprle_length, tvb, start_offset + 3, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_nvramdumprle_data, tvb, start_offset + 4, tvb_get_guint8 (tvb, start_offset + 3), ENC_NA);
}
