static guint
fPrescale (tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, guint offset)
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
        switch (tag_no) {
        case 0: /
            offset = fUnsignedTag (tvb,tree,offset,"Multiplier: ");
            break;
        case 1: /
            offset = fUnsignedTag (tvb,tree,offset,"Modulo Divide: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;

}
