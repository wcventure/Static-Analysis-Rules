static guint
fSignedTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    gint64      val = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (fSigned64 (tvb, offset + tag_len, lvt, &val))
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s(Signed) %" G_GINT64_MODIFIER "d", label, val);
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s - %u octets (Signed)", label, lvt);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
