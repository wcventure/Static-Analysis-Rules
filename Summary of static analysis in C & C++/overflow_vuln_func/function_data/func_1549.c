static void
dissect_skinny_xml(proto_tree *tree, tvbuff_t *tvb, packet_info *pinfo, const gint start, gint length)
{
  proto_item         *item    = NULL;
  proto_tree         *subtree = NULL;
  tvbuff_t           *next_tvb;
  dissector_handle_t  handle;

  item = proto_tree_add_item(tree, hf_skinny_xmlData, tvb, start, length, ENC_ASCII|ENC_NA);
  subtree = proto_item_add_subtree(item, 0);
  next_tvb = tvb_new_subset(tvb, start, length, -1);
  handle = dissector_get_string_handle(media_type_dissector_table, "text/xml");
  if (handle != NULL) {
    call_dissector(handle, next_tvb, pinfo, subtree);
   }
}
