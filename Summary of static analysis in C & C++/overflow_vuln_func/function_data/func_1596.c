static void dissect_r3_cmd_response (tvbuff_t *tvb, guint32 start_offset, guint32 length, packet_info *pinfo, proto_tree *tree)
{
  guint8 responseLen = tvb_get_guint8 (tvb, start_offset + 0);
  responseType_e responseType = tvb_get_guint8 (tvb, start_offset + 2);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset, responseLen, responseLen);
  const gchar *rt = NULL;

  rt = val_to_str_ext_const (responseType, &r3_responsetypenames_ext, "[Unknown Response Type]");

  proto_item_set_text (proto_tree_get_parent (tree), "Response Packet: %s (%u)", rt, responseType);
  proto_tree_add_item (tree, hf_r3_responselength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_responsecommand, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  if (tvb_get_guint8 (tvb, start_offset + 2) >= RESPONSETYPE_LAST)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "Octet 3 >= UPSTREAMCOMMAND_LAST");
  else if (r3response_dissect [responseType])
    (*r3response_dissect [responseType]) (payload_tvb, 0, length, pinfo, tree);
}
