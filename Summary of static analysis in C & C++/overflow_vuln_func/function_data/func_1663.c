static gint dissect_r3_command (tvbuff_t *tvb, guint32 start_offset, guint32 length, packet_info *pinfo, proto_tree *r3_tree)
{
  if (r3_tree)
  {
    proto_item *cmd_item = NULL;
    proto_tree *cmd_tree = NULL;
    const gchar *cn = NULL;
    guint8 cmdLen;
    guint8 cmd;

    tvb_ensure_bytes_exist (tvb, start_offset, 2);

    cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
    cmd = tvb_get_guint8 (tvb, start_offset + 1);

    if (!mfgCommandFlag)
      cn = val_to_str_ext_const (cmd, &r3_cmdnames_ext, "[Unknown Command]");
    else
      cn = val_to_str_ext_const (cmd, &r3_cmdmfgnames_ext, "[Unknown Mfg Command]");

    cmd_item = proto_tree_add_text (r3_tree, tvb, start_offset, cmdLen, "Command Packet: %s (%d)", cn, cmd);
    cmd_tree = proto_item_add_subtree (cmd_item, ett_r3cmd);

    if (cmd_tree)
    {
      if (!mfgCommandFlag)
      {
        if (cmd >= CMD_LAST)
          expert_add_info_format (pinfo, cmd_tree, PI_UNDECODED, PI_WARN, "Unknown command value");
        else if (r3command_dissect [cmd])
          (*r3command_dissect [cmd]) (tvb, start_offset, length, pinfo, cmd_tree);
      }
      else
      {
        mfgCommandFlag = FALSE;

        if (cmd >= CMDMFG_LAST)
          expert_add_info_format (pinfo, cmd_tree, PI_UNDECODED, PI_WARN, "Unknown manufacturing command value");
        else if (r3commandmfg_dissect [cmd])
          (*r3commandmfg_dissect [cmd]) (tvb, start_offset, length, pinfo, cmd_tree);
      }
    }
  }

  return tvb_get_guint8 (tvb, start_offset);
}
