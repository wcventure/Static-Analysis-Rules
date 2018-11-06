static guint
fWritePropertyMultipleError(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no     = 0, tag_info = 0;
    guint32 lvt        = 0;

    col_set_writable(pinfo->cinfo, FALSE); /
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fContextTaggedError(tvb, pinfo, tree, offset);
            break;
        case 1: /
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fBACnetObjectPropertyReference(tvb, pinfo, tree, offset);
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
