static guint
fReadRangeRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    offset = fBACnetObjectPropertyReference(tvb, pinfo, subtree, offset);

    if (tvb_reported_length_remaining(tvb, offset) > 0) {
        /
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_opening(tag_info)) {
            tt = proto_tree_add_text(subtree, tvb, offset, 1, "%s", val_to_str(tag_no, BACnetReadRangeOptions, "unknown range option"));
            subtree = proto_item_add_subtree(tt, ett_bacapp_value);
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            switch (tag_no) {
            case 3: /
            case 6: /
                offset = fApplicationTypes (tvb, pinfo, subtree, offset, "reference Index: ");
                offset = fApplicationTypes (tvb, pinfo, subtree, offset, "reference Count: ");
                break;
            case 4: /
            case 7: /
                offset = fDateTime(tvb, subtree, offset, "reference Date/Time: ");
                offset = fApplicationTypes (tvb, pinfo, subtree, offset, "reference Count: ");
                break;
            case 5: /
                offset = fDateTime(tvb, subtree, offset, "beginning Time: ");
                offset = fDateTime(tvb, subtree, offset, "ending Time: ");
                break;
            default:
                break;
            }
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        }
    }
    return offset;
}
