static void dissect_r3_upstreammfgfield_checkpointlog (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *cpl_item = NULL;
  proto_tree *cpl_tree = NULL;
  guint counter = 0;
  guint l = tvb_length_remaining (tvb, start_offset + 1);
  guint i;

  proto_tree_add_item (tree, hf_r3_checkpointlog_entryptr, tvb, start_offset, 1, ENC_LITTLE_ENDIAN);
  cpl_item = proto_tree_add_text (tree, tvb, start_offset + 1, -1, "Checkpoint Log");
  cpl_tree = proto_item_add_subtree (cpl_item, ett_r3checkpointlog);

  for (start_offset++, i = 0; i < l; i += 2, counter++)
  {
    guint rcon = tvb_get_guint8 (tvb, start_offset + i + 0);
    guint cp = tvb_get_guint8 (tvb, start_offset + i + 1);
    guint resettype = 0;
    proto_item *cpe_item = proto_tree_add_text (cpl_tree, tvb, start_offset + i + 0, 2, "Checkpoint Log Entry %u", counter);
    proto_tree *cpe_tree = proto_item_add_subtree (cpe_item, ett_r3checkpointlogentry);
    const gchar *resets [] = { "Stack underflow", "Stack overflow", "Power-On", "Software", "Brown-out", "MCLR in sleep", "WDT", "Normal", "[Unknown Reset Type]" };
    const gchar *desc;

    desc = val_to_str_ext_const (cp, &r3_checkpointnames_ext, "[Unknown Checkpoint]");

    if (rcon == 0xff)
      resettype = 8;
    else
    {
      rcon &= 0x1f;

      if (rcon == 0x1c)
        resettype = 2;
      else if ((rcon & 0x10) == 0x00)
        resettype = 3;
      else if ((rcon & 0x1d) == 0x1c)
        resettype = 4;
      else if ((rcon & 0x0c) == 0x08)
        resettype = 5;
      else if ((rcon & 0x0c) == 0x04)
        resettype = 6;
      else
        resettype = 7;
    }

    proto_item_append_text (cpe_item, " (%s, %s)", resets [resettype], desc);
    proto_item_append_text (proto_tree_add_item (cpe_tree, hf_r3_checkpointlog_rcon, tvb, start_offset + i + 0, 1, ENC_LITTLE_ENDIAN), " (%s)", resets [resettype]);
    proto_item_append_text (proto_tree_add_item (cpe_tree, hf_r3_checkpointlog_checkpoint, tvb, start_offset + i + 1, 1, ENC_LITTLE_ENDIAN), " (%s)", desc);
  }
}
