static guint
flistOfEventSummaries (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree* subtree = tree;
    proto_item* ti = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            break;
        }
        switch (tag_no) {
        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 1: /
            offset = fEnumeratedTag (tvb, tree, offset,
                "event State: ", BACnetEventState);
            break;
        case 2: /
            offset = fBitStringTagVS (tvb, tree, offset,
                "acknowledged Transitions: ", BACnetEventTransitionBits);
            break;
        case 3: /
            ti = proto_tree_add_text(tree, tvb, offset, lvt, "eventTimeStamps");
            if (ti) {
                subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
            }
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            offset  = fTimeStamp (tvb, subtree, offset,"TO-OFFNORMAL timestamp: ");
            offset  = fTimeStamp (tvb, subtree, offset,"TO-FAULT timestamp: ");
            offset  = fTimeStamp (tvb, subtree, offset,"TO-NORMAL timestamp: ");
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 4: /
            offset  = fEnumeratedTag (tvb, tree, offset,
                "Notify Type: ", BACnetNotifyType);
            break;
        case 5: /
            offset  = fBitStringTagVS (tvb, tree, offset,
                "event Enable: ", BACnetEventTransitionBits);
            break;
        case 6: /
            ti = proto_tree_add_text(tree, tvb, offset, lvt, "eventPriorities");
            if (ti) {
                subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
            }
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            offset  = fUnsignedTag (tvb, subtree, offset, "TO-OFFNORMAL Priority: ");
            offset  = fUnsignedTag (tvb, subtree, offset, "TO-FAULT Priority: ");
            offset  = fUnsignedTag (tvb, subtree, offset, "TO-NORMAL Priority: ");
            offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
