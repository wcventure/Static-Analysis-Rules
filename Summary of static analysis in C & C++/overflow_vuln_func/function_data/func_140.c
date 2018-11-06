static guint
fContextTaggedValue(tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;
    gint        tvb_len;

    (void)label;
    tag_len = fTagHeader(tvb, offset, &tag_no, &tag_info, &lvt);
    /
    tvb_len = tvb_reported_length_remaining(tvb, offset+tag_len);
    if ((tvb_len >= 0) && ((guint32)tvb_len < lvt)) {
        lvt = tvb_len;
    }
    ti = proto_tree_add_text(tree, tvb, offset+tag_len, lvt,
        "Context Value (as %u DATA octets)", lvt);

    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset + tag_len + lvt;
}
