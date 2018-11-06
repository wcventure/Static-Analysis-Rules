static guint
fGetEventInformationACK (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = flistOfEventSummaries (tvb, pinfo, tree, offset);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 1: /
            offset  = fBooleanTag (tvb, tree, offset, "more Events: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
