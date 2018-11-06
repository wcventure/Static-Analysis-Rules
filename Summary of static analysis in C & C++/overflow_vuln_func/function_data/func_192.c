static guint
fReadAccessSpecification (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_item *tt;
    proto_tree *subtree = tree;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        switch (tag_no) {
        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 1: /
            if (tag_is_opening(tag_info)) {
                tt      = proto_tree_add_text(subtree, tvb, offset, 1, "listOfPropertyReferences");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fBACnetPropertyReference (tvb, pinfo, subtree, offset, 1);
            } else if (tag_is_closing(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset,
                    &tag_no, &tag_info, &lvt);
                subtree = tree;
            } else {
                /
                return offset;
            }
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
