static guint
fCreateObjectRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *subtree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

        if (tag_no < 2) {
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            switch (tag_no) {
            case 0: /
                switch (fTagNo(tvb, offset)) { /
                case 0: /
                    offset = fEnumeratedTagSplit (tvb, subtree, offset, "Object Type: ", BACnetObjectType, 128);
                    break;
                case 1: /
                    offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
                    break;
                default:
                    break;
                }
                break;
            case 1: /
                if (tag_is_opening(tag_info)) {
                    offset = fBACnetPropertyValue (tvb, pinfo, subtree, offset);
                    break;
                }
                FAULT;
                break;
            default:
                break;
            }
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
        }
        if (offset == lastoffset) break;    /
    }
    return offset;
}
