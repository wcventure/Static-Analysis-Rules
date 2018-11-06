static void dissect_r3_upstreammfgfield_peekpoke (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint i;
  guint l = tvb_length_remaining (tvb, start_offset);

  for (i = 0; i < l; i += 3)
  {
    proto_item *peekpoke_item = proto_tree_add_text (tree, tvb, start_offset + i, 3, "%s", "");
    proto_tree *peekpoke_tree = proto_item_add_subtree (peekpoke_item, ett_r3peekpoke);

    proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_operation, tvb, start_offset + i + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_address, tvb, start_offset + i + 1, 2, ENC_LITTLE_ENDIAN);

    switch (tvb_get_guint8 (tvb, start_offset + i + 0))
    {
      case PEEKPOKE_READU8 :
        proto_item_append_text (peekpoke_item, "Read (8 Bits @ 0x%04x = 0x%02x)", tvb_get_letohs (tvb, start_offset + i + 1), tvb_get_guint8 (tvb, start_offset + i + 3));
        proto_item_set_len (peekpoke_item, 4);
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_poke8, tvb, start_offset + i + 3, 1, ENC_LITTLE_ENDIAN);
        i += 1;
        break;

      case PEEKPOKE_READU16 :
        proto_item_append_text (peekpoke_item, "Read (16 Bits @ 0x%04x = 0x%04x)", tvb_get_letohs (tvb, start_offset + i + 1), tvb_get_letohs (tvb, start_offset + i + 3));
        proto_item_set_len (peekpoke_item, 5);
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_poke16, tvb, start_offset + i + 3, 2, ENC_LITTLE_ENDIAN);
        i += 2;
        break;

      case PEEKPOKE_READU24 :
        proto_item_append_text (peekpoke_item, "Read (24 Bits @ 0x%04x = 0x%06x)", tvb_get_letohs (tvb, start_offset + i + 1), tvb_get_letoh24 (tvb, start_offset + i + 3));
        proto_item_set_len (peekpoke_item, 6);
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_poke24, tvb, start_offset + i + 3, 3, ENC_LITTLE_ENDIAN);
        i += 3;
        break;

      case PEEKPOKE_READU32 :
        proto_item_append_text (peekpoke_item, "Read (32 Bits @ 0x%04x = 0x%08x)", tvb_get_letohs (tvb, start_offset + i + 1), tvb_get_letohl (tvb, start_offset + i + 3));
        proto_item_set_len (peekpoke_item, 7);
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_poke32, tvb, start_offset + i + 3, 4, ENC_LITTLE_ENDIAN);
        i += 4;
        break;

      case PEEKPOKE_READSTRING :
        proto_item_append_text (peekpoke_item, "Read (%u Bytes @ 0x%04x)", tvb_get_guint8 (tvb, start_offset + i + 3), tvb_get_letohs (tvb, start_offset + i + 1));
        proto_item_set_len (peekpoke_item, 3 + 1 + tvb_get_guint8 (tvb, start_offset + i + 3));
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_length, tvb, start_offset + i + 3, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item (peekpoke_tree, hf_r3_peekpoke_pokestring, tvb, start_offset + i + 4, tvb_get_guint8 (tvb, start_offset + i + 3), ENC_NA);
        i += tvb_get_guint8 (tvb, start_offset + i + 3) + 1;
        break;

      default :
        DISSECTOR_ASSERT (0);
    }
  }
}
