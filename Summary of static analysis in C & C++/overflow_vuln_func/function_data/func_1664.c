static gint dissect_r3_packet (tvbuff_t *tvb, guint start_offset, packet_info *pinfo, proto_tree *r3_tree)
{
  guint offset = 0;
  guint octConsumed;
  gint available = tvb_length_remaining (tvb, start_offset);

  if (!(tvb_strneql (tvb, start_offset, "~~~ds", 5)))
  {
    if (r3_tree)
      proto_tree_add_item (r3_tree, hf_r3_tildex3ds, tvb, start_offset + 0, -1, ENC_ASCII|ENC_NA);

    offset = start_offset + 5;
  }
  else
  {
    gint packetLen = 0;

    if (available < 4)
      return available - 4;
    else
    {
      packetLen = tvb_get_guint8 (tvb, start_offset + 3) + 1;

      if (packetLen > available)
        return available - packetLen;
    }

    --packetLen;

    /
    if (r3_tree)
    {
      proto_item *header_item = proto_tree_add_item (r3_tree, hf_r3_header, tvb, start_offset + 0, 5, ENC_NA);
      proto_tree *header_tree = proto_item_add_subtree (header_item, ett_r3header);

      proto_tree_add_item (header_tree, hf_r3_sigil, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (header_tree, hf_r3_address, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (header_tree, hf_r3_packetnumber, tvb, start_offset + 2, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (header_tree, hf_r3_packetlength, tvb, start_offset + 3, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (header_tree, hf_r3_encryption, tvb, start_offset + 4, 1, ENC_LITTLE_ENDIAN);
    }

    /
    if (available > 8)
    {
      proto_item *payload_item = NULL;
      proto_tree *payload_tree = NULL;
      tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 5, packetLen - 7, packetLen - 7);

      if (r3_tree)
      {
        payload_item = proto_tree_add_item (r3_tree, hf_r3_payload, payload_tvb, 0, -1, ENC_NA);
        payload_tree = proto_item_add_subtree (payload_item, ett_r3payload);
      }

      while (offset < tvb_reported_length (payload_tvb))
      {
        octConsumed = dissect_r3_command (payload_tvb, offset, 0, pinfo, payload_tree);
        if(!octConsumed)
        {
              expert_add_info_format (pinfo, payload_tree, PI_MALFORMED, PI_WARN, "Command length equal to 0; payload could be partially decoded");
              offset = tvb_reported_length (payload_tvb);
              break;
        }
        offset += octConsumed;
      }
    }

    offset += start_offset;

    /
    if (r3_tree)
    {
      proto_item *tail_item = proto_tree_add_item (r3_tree, hf_r3_tail, tvb, offset + 5, 3, ENC_NA);
      proto_tree *tail_tree = proto_item_add_subtree (tail_item, ett_r3tail);
      guint32 packetCRC = tvb_get_letohs (tvb, offset + 5);
      guint32 packetXor = tvb_get_guint8 (tvb, offset + 7);
      guint32 calculatedCRC = 0;

      if ((calculatedCRC = utilCrcCalculate (tvb_get_ptr (tvb, start_offset + 1, packetLen - 3), packetLen - 3, 0x0000)) == packetCRC)
        proto_tree_add_uint_format (tail_tree, hf_r3_crc, tvb, offset + 5, 2, packetCRC, "CRC: 0x%04x (correct)", packetCRC);
      else
      {
        proto_item *tmp_item;

        proto_tree_add_uint_format (tail_tree, hf_r3_crc, tvb, offset + 5, 2, packetCRC, "CRC: 0x%04x (incorrect, should be 0x%04x)", calculatedCRC, packetCRC);
        tmp_item = proto_tree_add_boolean (tail_tree, hf_r3_crc_bad, tvb, offset + 5, 2, TRUE);
        PROTO_ITEM_SET_GENERATED (tmp_item);
      }

      if ((packetLen ^ 0xff) == (int)packetXor)
        proto_tree_add_uint_format (tail_tree, hf_r3_xor, tvb, offset + 7, 1, packetXor, "XOR: 0x%02x (correct)", packetXor);
      else
      {
        proto_item *tmp_item;

        proto_tree_add_uint_format (tail_tree, hf_r3_xor, tvb, offset + 7, 1, packetXor, "XOR: 0x%02x (incorrect, should be 0x%02x)", packetXor, packetLen ^ 0xff);
        tmp_item = proto_tree_add_boolean (tail_tree, hf_r3_xor_bad, tvb, offset + 7, 1, TRUE);
        PROTO_ITEM_SET_GENERATED (tmp_item);
      }
    }

    offset += 8;
  }

  return offset - start_offset;
}
