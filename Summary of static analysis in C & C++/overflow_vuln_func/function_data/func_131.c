static guint
fActionCommand (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, guint8 tag_match)
{
    guint       lastoffset = 0, len;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree    = tree;

    /
    propertyArrayIndex = -1;
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info) ) {
            if (tag_no == tag_match) {
                return offset;
            }
            offset += len;
            subtree = tree;
            continue;
        }
        switch (tag_no) {

        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 1: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 2: /
            offset = fPropertyIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 3: /
            offset = fPropertyArrayIndex (tvb, subtree, offset);
            break;
        case 4: /
            offset = fPropertyValue (tvb, pinfo, subtree, offset, tag_info);
            break;
        case 5: /
            offset = fUnsignedTag (tvb,subtree,offset,"Priority: ");
            break;
        case 6: /
            offset = fUnsignedTag (tvb,subtree,offset,"Post Delay: ");
            break;
        case 7: /
            offset = fBooleanTag(tvb, subtree, offset,
                "Quit On Failure: ");
            break;
        case 8: /
            offset = fBooleanTag(tvb, subtree, offset,
                "Write Successful: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
