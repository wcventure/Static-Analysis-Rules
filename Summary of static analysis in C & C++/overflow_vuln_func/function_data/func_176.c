static guint
fLOPR (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    col_set_writable(pinfo->cinfo, FALSE); /
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            break;
        }
        offset = fDeviceObjectPropertyReference(tvb, pinfo, tree, offset);
        if (offset == lastoffset) break;     /
    }
    return offset;
}
