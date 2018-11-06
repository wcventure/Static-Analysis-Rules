static guintdissect_bfd_control
fEventLogRecord(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fDate (tvb,tree,offset,"Date: ");
            offset  = fTime (tvb,tree,offset,"Time: ");
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 1: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            switch (fTagNo(tvb, offset)) {
            case 0: /    /
                offset = fBitStringTagVS(tvb, tree, offset, "log status:", BACnetLogStatus);
                break;
            case 1: /
                tt = proto_tree_add_text(tree, tvb, offset, 1, "notification: ");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fConfirmedEventNotificationRequest(tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 2:
                offset = fRealTag (tvb, tree, offset, "time-change: ");
                break;
            default:
                return offset;
            }
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
