static guint
fReadRangeAck (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    /
    propertyArrayIndex = -1;
    /
    offset = fBACnetObjectPropertyReference(tvb, pinfo, subtree, offset);
    /
    offset = fBitStringTagVS (tvb, tree, offset,
        "resultFlags: ",
        BACnetResultFlags);
    /
    offset = fUnsignedTag (tvb, subtree, offset, "item Count: ");
    /
    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (tag_is_opening(tag_info)) {
        col_set_writable(pinfo->cinfo, FALSE); /
        tt = proto_tree_add_text(subtree, tvb, offset, 1, "itemData");
        subtree = proto_item_add_subtree(tt, ett_bacapp_value);
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        offset  = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    }
    /
    if (tvb_reported_length_remaining(tvb, offset) > 0) {
        offset  = fUnsignedTag (tvb, subtree, offset, "first Sequence Number: ");
    }

    return offset;
}
