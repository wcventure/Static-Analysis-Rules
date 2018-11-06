static guint
fOctetString (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label, guint32 lvt)
{
    gchar      *tmp;
    guint       start   = offset;
    guint8      tag_no, tag_info;
    proto_tree *subtree = tree;
    proto_item *ti      = 0;

    offset += fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

    if (lvt > 0) {
        tmp = tvb_bytes_to_str(tvb, offset, lvt);
        ti = proto_tree_add_text(tree, tvb, offset, lvt, "%s %s", label, tmp);
        offset += lvt;
    }

    if (ti)
        subtree = proto_item_add_subtree(ti, ett_bacapp_tag);

    fTagHeaderTree(tvb, subtree, start, &tag_no, &tag_info, &lvt);

    return offset;
}
