static void dissect_r3_cmd_downloadfirmware (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *dlfw_item = NULL;
  proto_tree *dlfw_tree = NULL;
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 packetCRC = 0;
  guint32 calculatedCRC = 0;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  dlfw_item = proto_tree_add_text (tree, payload_tvb, 0, -1, "Download Record (Record #%u, ", tvb_get_letohs (payload_tvb, 2));
  dlfw_tree = proto_item_add_subtree (dlfw_item, ett_r3downloadfirmware);

  proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_length, payload_tvb, 0, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_record, payload_tvb, 2, 2, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_action, payload_tvb, 4, 1, ENC_LITTLE_ENDIAN);

  switch (tvb_get_guint8 (payload_tvb, 4))
  {
    case DOWNLOADFIRMWARE_START :
      proto_item_append_text (dlfw_item, "DOWNLOADFIRMWARE_START)");
      proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_timeout, payload_tvb, 5, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_nvram, payload_tvb, 6, 1, ENC_LITTLE_ENDIAN);
      break;

    case DOWNLOADFIRMWARE_DATA :
      proto_item_append_text (dlfw_item, "DOWNLOADFIRMWARE_DATA, Address 0x%08x, %u Bytes)", tvb_get_letohl (payload_tvb, 5), tvb_get_guint8 (payload_tvb, 9));
      proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_address, payload_tvb, 5, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_bytes, payload_tvb, 9, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (dlfw_tree, hf_r3_firmwaredownload_data, payload_tvb, 10, tvb_get_guint8 (payload_tvb, 9), ENC_NA);
      break;

    case DOWNLOADFIRMWARE_COMPLETE :
      proto_item_append_text (dlfw_item, "DOWNLOADFIRMWARE_COMPLETE)");
      break;

    case DOWNLOADFIRMWARE_ABORT :
      proto_item_append_text (dlfw_item, "DOWNLOADFIRMWARE_ABORT)");
      break;

    case DOWNLOADFIRMWARE_RESET :
      proto_item_append_text (dlfw_item, "DOWNLOADFIRMWARE_RESET)");
      break;

    default :
      DISSECTOR_ASSERT (0);
  }

  packetCRC = tvb_get_letohs (payload_tvb, tvb_reported_length (payload_tvb) - 2);

  if ((calculatedCRC = utilCrcCalculate (tvb_get_ptr (payload_tvb, 0, tvb_reported_length (payload_tvb) - 2), tvb_reported_length (payload_tvb) - 2, 0x0000)) == packetCRC)
    proto_tree_add_uint_format (dlfw_tree, hf_r3_firmwaredownload_crc, payload_tvb, tvb_reported_length (payload_tvb) - 2, 2, packetCRC, "CRC: 0x%04x (correct)", packetCRC);
  else
  {
    proto_item *tmp_item;

    proto_tree_add_uint_format (dlfw_tree, hf_r3_firmwaredownload_crc, payload_tvb, tvb_reported_length (payload_tvb) - 2, 2, packetCRC, "CRC: 0x%04x (incorrect, should be 0x%04x)", calculatedCRC, packetCRC);
    tmp_item = proto_tree_add_boolean (dlfw_tree, hf_r3_firmwaredownload_crc_bad, payload_tvb, tvb_reported_length (payload_tvb) - 2, 2, TRUE);
    PROTO_ITEM_SET_GENERATED (tmp_item);
  }
}
