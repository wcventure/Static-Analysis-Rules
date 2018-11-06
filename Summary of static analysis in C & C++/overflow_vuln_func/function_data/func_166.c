static guint
fNotificationParameters (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = offset;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    tt = proto_tree_add_text(subtree, tvb, offset, 0, "notification parameters (%d) %s",
        tag_no, val_to_str(tag_no, BACnetEventType, "invalid type"));
    subtree = proto_item_add_subtree(tt, ett_bacapp_value);
    /
    offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    switch (tag_no) {
    case 0: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fBitStringTag (tvb, subtree, offset,
                    "referenced-bitstring: ");
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 1: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fBACnetPropertyStates(tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 2: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                switch (fTagNo(tvb, offset)) {
                case 0:
                    offset = fBitStringTag (tvb, subtree, offset,
                        "changed-bits: ");
                    break;
                case 1:
                    offset = fRealTag (tvb, subtree, offset,
                        "changed-value: ");
                    break;
                default:
                    break;
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 3: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0: /
                /
                propertyIdentifier = 85; /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                break;
            case 2: /
                propertyIdentifier = 40; /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 4: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fRealTag (tvb, subtree, offset, "reference-value: ");
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                break;
            case 2:
                offset = fRealTag (tvb, subtree, offset, "setpoint-value: ");
                break;
            case 3:
                offset = fRealTag (tvb, subtree, offset, "error-limit: ");
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 5: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fRealTag (tvb, subtree, offset, "exceeding-value: ");
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                break;
            case 2:
                offset = fRealTag (tvb, subtree, offset, "deadband: ");
                break;
            case 3:
                offset = fRealTag (tvb, subtree, offset, "exceeded-limit: ");
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 6:
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            offset =fBACnetPropertyValue (tvb,pinfo,subtree,offset);
            if (offset == lastoffset) break;     /
        }
        break;
    case 7: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fObjectIdentifier (tvb, pinfo, subtree, offset); /
                break;
            case 1:
                offset = fObjectIdentifier (tvb, pinfo, subtree, offset); /
                break;
            case 2:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDateTime (tvb, subtree, offset, "previous-notification: ");
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 3:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDateTime (tvb, subtree, offset, "current-notification: ");
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 8: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fEnumeratedTagSplit (tvb, subtree, offset,
                    "new-state: ", BACnetLifeSafetyState, 256);
                break;
            case 1:
                offset = fEnumeratedTagSplit (tvb, subtree, offset,
                    "new-mode: ", BACnetLifeSafetyMode, 256);
                break;
            case 2:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                break;
            case 3:
                offset = fEnumeratedTagSplit (tvb, subtree, offset,
                    "operation-expected: ", BACnetLifeSafetyOperation, 64);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 9: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fVendorIdentifier (tvb, pinfo, subtree, offset);
                break;
            case 1:
                offset = fUnsignedTag (tvb, subtree, offset,
                    "extended-event-type: ");
                break;
            case 2: /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fApplicationTypes(tvb, pinfo, subtree, offset, "parameters: ");
                offset  = fDeviceObjectPropertyValue(tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 10: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0: /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDeviceObjectPropertyReference (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 1:
                offset  = fUnsignedTag (tvb, subtree, offset,
                    "previous-notification: ");
                break;
            case 2:
                offset = fUnsignedTag (tvb, subtree, offset,
                    "current-notification: ");
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 11: /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fUnsignedTag (tvb, subtree, offset,
                    "exceeding-value: ");
                break;
            case 1:
                offset = fBitStringTagVS (tvb, subtree, offset,
                    "status-flags: ", BACnetStatusFlags);
                break;
            case 2:
                offset = fUnsignedTag (tvb, subtree, offset,
                    "exceeded-limit: ");
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
        /
    default:
        offset = fAbstractSyntaxNType(tvb, pinfo, subtree, offset);
        break;
    }

    /
    offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset;
}
