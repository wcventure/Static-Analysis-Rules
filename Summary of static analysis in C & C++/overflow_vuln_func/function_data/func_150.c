static guint
fBACnetPropertyValue (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        offset = fPropertyIdentifierValue(tvb, pinfo, tree, offset, 0);
        if (offset > lastoffset) {
            /
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_context_specific(tag_info) && (tag_no == 3))
                offset = fUnsignedTag (tvb,tree,offset,"Priority: ");
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
