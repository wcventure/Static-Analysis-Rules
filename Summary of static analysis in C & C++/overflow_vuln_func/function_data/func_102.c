static guint
fNullTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_item *ti;
    proto_tree *subtree;

    ti      = proto_tree_add_text(tree, tvb, offset, 1, "%sNULL", label);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset + 1;
}
