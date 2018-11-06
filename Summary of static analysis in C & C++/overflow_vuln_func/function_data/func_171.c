static guint
fConfirmedEventNotificationRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) {
            lastoffset = offset;
            break;
        }

        switch (tag_no) {
        case 0: /
            offset  = fProcessId (tvb,tree,offset);
            break;
        case 1: /
            offset  = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 2: /
            offset  = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 3: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fTimeStamp (tvb, tree, offset, NULL);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 4: /
            offset  = fUnsignedTag (tvb, tree, offset, "Notification Class: ");
            break;
        case 5: /
            offset  = fUnsignedTag (tvb, tree, offset, "Priority: ");
            break;
        case 6: /
            offset  = fEnumeratedTagSplit (tvb, tree, offset,
                "Event Type: ", BACnetEventType, 64);
            break;
        case 7: /
            offset  = fCharacterString (tvb, tree, offset, "message Text: ");
            break;
        case 8: /
            offset  = fEnumeratedTag (tvb, tree, offset,
                "Notify Type: ", BACnetNotifyType);
            break;
        case 9: /
            offset  = fBooleanTag (tvb, tree, offset, "ack Required: ");
            break;
        case 10: /
            offset  = fEnumeratedTagSplit (tvb, tree, offset,
                "from State: ", BACnetEventState, 64);
            break;
        case 11: /
            offset  = fEnumeratedTagSplit (tvb, tree, offset,
                "to State: ", BACnetEventState, 64);
            break;
        case 12: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fNotificationParameters (tvb, pinfo, tree, offset);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            break;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
