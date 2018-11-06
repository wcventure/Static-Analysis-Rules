static guint
fBooleanTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8      tag_no, tag_info;
    guint32     lvt      = 0;
    proto_item *ti;
    proto_tree *subtree;
    guint       bool_len = 1;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (tag_info && lvt == 1) {
        lvt = tvb_get_guint8(tvb, offset+1);
        ++bool_len;
    }

    ti = proto_tree_add_text(tree, tvb, offset, bool_len,
                             "%s%s", label, lvt == 0 ? "FALSE" : "TRUE");
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset + bool_len;
}
