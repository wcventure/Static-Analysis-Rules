static guint
fDoorMembers (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8  tag_no, tag_info;
    guint32 lvt;
    guint   lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info) ) {
            return offset;
            }
        offset = fDeviceObjectReference(tvb, pinfo, tree, offset);
        if (offset == lastoffset) break;
    }
    return offset;
}
