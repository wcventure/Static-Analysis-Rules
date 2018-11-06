static void
dissect_zcl_appl_ctrl_wr_funcs(tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
  proto_item  *ti = NULL;
  proto_tree  *sub_tree = NULL;
  guint tvb_len;
  guint i = 0;

  tvb_len = tvb_reported_length(tvb);
  while ( *offset < tvb_len && i < ZBEE_ZCL_NUM_ATTR_ETT ) {
    /
    ti = proto_tree_add_text(tree, tvb, *offset, 0, "Function #%d", i);
    sub_tree = proto_item_add_subtree(ti, ett_zbee_zcl_appl_ctrl_func[i]);
    i++;

    /
    dissect_zcl_appl_ctrl_attr_func(tvb, sub_tree, offset);
  }

} /
