static guint
fWriteAccessSpecification (tvbuff_t *tvb, packet_info *pinfo, proto_tree *subtree, guint offset)
{
    guint   lastoffset = 0, len;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            offset += len;
            continue;
        }

        switch (tag_no) {
        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 1: /
            if (tag_is_opening(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fBACnetPropertyValue (tvb, pinfo, subtree, offset);
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
