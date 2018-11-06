static guint
fAcknowledgeAlarmRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no = 0, tag_info = 0;
    guint32 lvt = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fUnsignedTag (tvb, tree, offset, "acknowledging Process Id: ");
            break;
        case 1: /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 2: /
            offset = fEnumeratedTagSplit (tvb, tree, offset,
                "event State Acknowledged: ", BACnetEventState, 64);
            break;
        case 3: /
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fTimeStamp(tvb, tree, offset, NULL);
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 4: /
            offset  = fCharacterString (tvb, tree, offset, "acknowledgement Source: ");
            break;
        case 5: /
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fTimeStamp(tvb, tree, offset, "acknowledgement timestamp: ");
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
