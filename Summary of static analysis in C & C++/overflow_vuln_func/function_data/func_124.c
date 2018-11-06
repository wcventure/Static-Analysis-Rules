static guint
fAddress (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint8  tag_no, tag_info;
    guint32 lvt;
    guint   offs;

    offset = fUnsignedTag (tvb, tree, offset, "network-number");
    offs   = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (lvt == 0) {
        proto_tree_add_text(tree, tvb, offset, offs, "MAC-address: broadcast");
        offset += offs;
    } else
        offset  = fMacAddress (tvb, tree, offset, "MAC-address: ", lvt);

    return offset;
}
