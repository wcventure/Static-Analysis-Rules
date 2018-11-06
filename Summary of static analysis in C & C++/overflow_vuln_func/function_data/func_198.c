static guint
fAtomicReadFileRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    offset = fObjectIdentifier (tvb, pinfo, tree, offset);

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

    if (tag_is_opening(tag_info)) {
        tt = proto_tree_add_text(subtree, tvb, offset, 1, "%s", val_to_str(tag_no, BACnetFileAccessOption, "unknown access method"));
        subtree = proto_item_add_subtree(tt, ett_bacapp_value);
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        offset  = fSignedTag (tvb, subtree, offset, val_to_str(tag_no, BACnetFileStartOption, "unknown option"));
        offset  = fUnsignedTag (tvb, subtree, offset, val_to_str(tag_no, BACnetFileRequestCount, "unknown option"));
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    }
    return offset;
}
