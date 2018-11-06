static guint
fObjectSelectionCriteria (tvbuff_t *tvb, packet_info *pinfo, proto_tree *subtree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            break;
        }

        switch (tag_no) {
        case 0: /
            offset = fEnumeratedTag (tvb, subtree, offset,
                "selection Logic: ", BACnetSelectionLogic);
            break;
        case 1: /
            if (tag_is_opening(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fSelectionCriteria (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            }
            FAULT;
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
