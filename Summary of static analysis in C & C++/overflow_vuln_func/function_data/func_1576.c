static void dissect_r3_upstreammfgfield_iopins (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo, proto_tree *tree)
{
  guint32 i;
  guint32 l = tvb_length_remaining (tvb, start_offset);
  char portname = 'A';

  if (l % 3 != 0)
    expert_add_info_format (pinfo, tree, PI_UNDECODED, PI_WARN, "IOPINS data length not modulo 3 == 0");
  else
  {
    for (i = 0; i < l; i += 3, portname++)
    {
      proto_item *port_item = proto_tree_add_text (tree, tvb, start_offset + i, 3, "Port %c Configuration", (portname == 'I') ? ++portname : portname);
      proto_tree *port_tree = proto_item_add_subtree (port_item, ett_r3iopins);

      proto_tree_add_item (port_tree, hf_r3_iopins_lat, tvb, start_offset + i + 0, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (port_tree, hf_r3_iopins_port, tvb, start_offset + i + 1, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item (port_tree, hf_r3_iopins_tris, tvb, start_offset + i + 2, 1, ENC_LITTLE_ENDIAN);
    }
  }
}
