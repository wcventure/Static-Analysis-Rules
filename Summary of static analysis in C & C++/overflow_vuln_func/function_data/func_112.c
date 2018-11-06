static guint
fTimeSpan (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint32     val = 0, lvt;
    guint8      tag_no, tag_info;
    proto_item *ti;
    proto_tree *subtree;
    guint       tag_len;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (fUnsigned32 (tvb, offset+tag_len, lvt, &val))
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
        "%s (hh.mm.ss): %d.%02d.%02d%s",
        label,
        (val / 3600), ((val % 3600) / 60), (val % 60),
        val == 0 ? " (indefinite)" : "");
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s - %u octets (Signed)", label, lvt);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
