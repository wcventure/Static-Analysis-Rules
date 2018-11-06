static void dissect_r3_response_hasdata (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo, proto_tree *tree)
{
  proto_item *upstreamcommand_item = NULL;
  proto_tree *upstreamcommand_tree = NULL;
  tvbuff_t *upstreamcommand_tvb;
  guint32 commandPacketLen;
  const gchar *ct = NULL;

  tvb_ensure_bytes_exist (tvb, start_offset, 4);
  commandPacketLen = tvb_get_guint8 (tvb, 0);
  upstreamcommand_tvb = tvb_new_subset (tvb, 4, commandPacketLen - 4, commandPacketLen - 4);

  if (tvb_get_guint8 (tvb, 1) != CMD_RESPONSE)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Octet 1 not CMD_RESPONSE");
  else if (tvb_get_guint8 (tvb, 2) != RESPONSETYPE_HASDATA)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Octet 2 not RESPONSE_HASDATA");
  else if (tvb_get_guint8 (tvb, 3) >= UPSTREAMCOMMAND_LAST)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Octet 3 >= UPSTREAMCOMMAND_LAST");
  else
  {
    ct = val_to_str_ext_const (tvb_get_guint8 (tvb, 3), &r3_upstreamcommandnames_ext, "[Unknown Command Type]");

    proto_tree_add_item (tree, hf_r3_responsetype, tvb, 2, 1, ENC_LITTLE_ENDIAN);

    upstreamcommand_item = proto_tree_add_text (tree, tvb, 3, -1, "Upstream Command: %s (%u)", ct, tvb_get_guint8 (tvb, 3));
    upstreamcommand_tree = proto_item_add_subtree (upstreamcommand_item, ett_r3upstreamcommand);

    proto_tree_add_item (upstreamcommand_tree, hf_r3_upstreamcommand, tvb, 3, 1, ENC_LITTLE_ENDIAN);

    tvb_ensure_bytes_exist (tvb, start_offset, commandPacketLen - 4);

    if (r3upstreamcommand_dissect [tvb_get_guint8 (tvb, 3)])
      (*r3upstreamcommand_dissect [tvb_get_guint8 (tvb, 3)]) (upstreamcommand_tvb, 0, commandPacketLen - 4, pinfo, upstreamcommand_tree);
  }
}
