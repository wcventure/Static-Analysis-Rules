static guint
fAccessMethod(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint32     lvt;
    guint8      tag_no, tag_info;
    proto_item* tt;
    proto_tree* subtree = NULL;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

    if (tag_is_opening(tag_info)) {
        tt = proto_tree_add_text(tree, tvb, offset, 1, "%s", val_to_str(tag_no, BACnetFileAccessOption, "invalid access method"));
        subtree = proto_item_add_subtree(tt, ett_bacapp_value);
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        offset  = fApplicationTypes (tvb, pinfo, subtree, offset, val_to_str(tag_no, BACnetFileStartOption, "invalid option"));
        offset  = fApplicationTypes (tvb, pinfo, subtree, offset, val_to_str(tag_no, BACnetFileWriteInfo, "unknown option"));

        if (tag_no == 1) {
            while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {
                /
                lastoffset = offset;
                offset = fApplicationTypes (tvb, pinfo, subtree, offset, "Record Data: ");
            }
        }

        if ((bacapp_flags & BACAPP_MORE_SEGMENTS) == 0) {
            /
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_closing(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            }
        }
    }
    return offset;
}
