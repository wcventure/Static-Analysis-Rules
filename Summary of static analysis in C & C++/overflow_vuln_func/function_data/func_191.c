static guint
fReadPropertyConditionalRequest(tvbuff_t *tvb, packet_info* pinfo, proto_tree *subtree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

        if (tag_is_opening(tag_info) && tag_no < 2) {
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            switch (tag_no) {
            case 0: /
                offset = fObjectSelectionCriteria (tvb, pinfo, subtree, offset);
                break;
            case 1: /
                offset = fBACnetPropertyReference (tvb, pinfo, subtree, offset, 1);
                break;
            default:
                return offset;
            }
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
