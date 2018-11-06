static guint
fWritePropertyRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;

    /
    propertyArrayIndex = -1;
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            subtree = tree;
            break;
        }

        switch (tag_no) {
        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 1: /
            offset = fPropertyIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 2: /
            offset = fPropertyArrayIndex (tvb, subtree, offset);
            break;
        case 3: /
            offset = fPropertyValue (tvb, pinfo, subtree, offset, tag_info);
            break;
        case 4: /
            offset = fUnsignedTag (tvb, subtree, offset, "Priority: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
