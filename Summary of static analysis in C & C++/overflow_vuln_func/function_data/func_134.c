static guint
fPropertyArrayIndex (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (fUnsigned32 (tvb, offset + tag_len, lvt, (guint32 *)&propertyArrayIndex))
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "property Array Index (Unsigned) %u", propertyArrayIndex);
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "property Array Index - %u octets (Unsigned)", lvt);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
