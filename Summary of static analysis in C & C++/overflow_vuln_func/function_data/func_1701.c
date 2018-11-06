static int dissect_dmflag(tvbuff_t *tvb, proto_tree *tree, guint16 offset) {
  proto_item *ti_local;
  proto_tree *local_tree;
  guint i;
  guint32 bit_offset=offset<<3;

  ti_local = proto_tree_add_item(tree, hf_reload_dmflags, tvb, offset, 64, ENC_BIG_ENDIAN);
  local_tree = proto_item_add_subtree(ti_local, ett_reload_dmflags);

  for (i=0; i<sizeof(reload_dmflag_items); i++) {
    if (reload_dmflag_items[i]!=NULL) {
      proto_tree_add_bits_item(local_tree, *(reload_dmflag_items[i]), tvb, bit_offset+63-i, 1, FALSE);
    }
  }
  return 8;
}
