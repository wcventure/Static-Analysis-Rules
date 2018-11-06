static void dissect_r3_upstreamcommand_debugmsg (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  proto_item *debugmsg_item = NULL;
  proto_tree *debugmsg_tree = NULL;

  debugmsg_item = proto_tree_add_text (tree, tvb, start_offset, -1, "Debug message");
  debugmsg_tree = proto_item_add_subtree (debugmsg_item, ett_r3debugmsg);

  proto_tree_add_item (debugmsg_tree, hf_r3_debugmsg, tvb, start_offset + 1, -1, ENC_ASCII|ENC_NA);
}
