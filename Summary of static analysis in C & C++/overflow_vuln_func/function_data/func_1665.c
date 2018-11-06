static void dissect_r3_message (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_item *r3_item = NULL;
  proto_tree *r3_tree = NULL;
  guint offset = 0;

  col_set_str (pinfo->cinfo, COL_PROTOCOL, "R3");
  col_clear (pinfo->cinfo, COL_INFO);

  if (tree)
  {
    r3_item = proto_tree_add_item (tree, proto_r3, tvb, 0, -1, ENC_NA);
    r3_tree = proto_item_add_subtree (r3_item, ett_r3);
  }

  while (offset < tvb_reported_length (tvb))
  {
    gint res = dissect_r3_packet (tvb, offset, pinfo, r3_tree);

    if (res <= 0)
    {
      if (r3_item)
        proto_item_set_len (r3_item, offset);

      return;
    }

    offset += res;
  }

  return;
}
