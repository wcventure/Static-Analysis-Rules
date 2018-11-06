static guint
fEventParameter (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = offset;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    tt = proto_tree_add_text(subtree, tvb, offset, 0, "event parameters (%d) %s",
        tag_no, val_to_str(tag_no, BACnetEventType, "invalid type"));
    subtree = proto_item_add_subtree(tt, ett_bacapp_value);
    /
    offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    switch (tag_no) {
    case 0: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_closing(tag_info)) {
                break;
            }
            switch (tag_no) {
            case 0:
                offset = fTimeSpan (tvb, subtree, offset, "Time Delay");
                break;
            case 1:
                offset = fBitStringTag (tvb, subtree, offset, "bitmask: ");
                break;
            case 2: /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
                    lastoffset = offset;
                    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
                    if (tag_is_closing(tag_info)) {
                        break;
                    }
                    offset = fBitStringTag(tvb, subtree, offset,
                                           "bitstring value: ");
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            default:
                break;
            }
        }
        break;
    case 1: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_closing(tag_info)) {
                break;
            }
            switch (tag_no) {
            case 0:
                offset = fTimeSpan (tvb, subtree, offset, "Time Delay");
                break;
            case 1: /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
                    lastoffset = offset;
                    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
                    if (tag_is_closing(tag_info)) {
                        break;
                    }
                    offset = fBACnetPropertyStates(tvb, pinfo, subtree, offset);
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            default:
                break;
            }
        }
        break;
    case 2: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fTimeSpan   (tvb, subtree, offset, "Time Delay");
                break;
            case 1: /
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                switch (fTagNo(tvb, offset)) {
                case 0:
                    offset = fBitStringTag (tvb, subtree, offset, "bitmask: ");
                    break;
                case 1:
                    offset = fRealTag (tvb, subtree, offset,
                                       "referenced Property Increment: ");
                    break;
                default:
                    break;
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            default:
                break;
            }
        }
        break;
    case 3: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            tag_no = fTagNo(tvb, offset);
            switch (tag_no) {
            case 0:
                offset = fTimeSpan   (tvb, subtree, offset, "Time Delay");
                break;
            case 1:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDeviceObjectPropertyReference (tvb,pinfo,subtree,offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            default:
                break;
            }
        }
        break;
    case 4: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_closing(tag_info)) {
                break;
            }
            switch (tag_no) {
            case 0:
                offset = fTimeSpan   (tvb, subtree, offset, "Time Delay");
                break;
            case 1:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDeviceObjectPropertyReference (tvb,pinfo,subtree,offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 2:
                offset  = fRealTag (tvb, subtree, offset, "low diff limit: ");
                break;
            case 3:
                offset  = fRealTag (tvb, subtree, offset, "high diff limit: ");
                break;
            case 4:
                offset  = fRealTag (tvb, subtree, offset, "deadband: ");
                break;
            default:
                break;
            }
        }
        break;
    case 5: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fTimeSpan (tvb, subtree, offset, "Time Delay");
                break;
            case 1:
                offset = fRealTag (tvb, subtree, offset, "low limit: ");
                break;
            case 2:
                offset = fRealTag (tvb, subtree, offset, "high limit: ");
                break;
            case 3:
                offset = fRealTag (tvb, subtree, offset, "deadband: ");
                break;
            default:
                break;
            }
        }
        break;
        /

    case 7: /
#if 0
        /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fUnsignedTag (tvb,tree,offset,"notification threshold");
                break;
            case 1:
                offset = fUnsignedTag (tvb,tree,offset,
                                       "previous notification count: ");
                break;
            default:
                return offset;
            }
        }
#endif
        break;
    case 8: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fTimeSpan (tvb, subtree, offset, "Time Delay");
                break;
            case 1:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
                    lastoffset = offset;
                    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
                    if (tag_is_closing(tag_info)) {
                        break;
                    }
                    offset = fEnumeratedTagSplit (tvb, subtree, offset,
                                                  "life safety alarm value: ", BACnetLifeSafetyState, 256);
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 2:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
                    lastoffset = offset;
                    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
                    if (tag_is_closing(tag_info)) {
                        break;
                    }
                    offset = fEnumeratedTagSplit (tvb, subtree, offset,
                                                  "alarm value: ", BACnetLifeSafetyState, 256);
                }
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 3:
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fDeviceObjectPropertyReference (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            default:
                break;
            }
        }
        break;
    case 9: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fVendorIdentifier (tvb, pinfo, tree, offset);
                break;
            case 1:
                offset = fUnsignedTag (tvb, tree, offset,
                                       "extended-event-type: ");
                break;
            case 2: /
                offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
                offset  = fApplicationTypes(tvb, pinfo, tree, offset, "parameters: ");
                offset  = fDeviceObjectPropertyValue(tvb, pinfo, tree, offset);
                offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
                lastoffset = offset;
                break;
            default:
                break;
            }
            if (offset == lastoffset) break;     /
        }
        break;
    case 10: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fUnsignedTag (tvb, subtree, offset,
                                       "notification-threshold: ");
                break;
            case 1:
                offset = fUnsignedTag (tvb, subtree, offset,
                                       "previous-notification-count: ");
                break;
            default:
                break;
            }
        }
        break;
    case 11: /
        while ((tvb_reported_length_remaining(tvb, offset) > 0)&&(offset>lastoffset)) {  /
            lastoffset = offset;
            switch (fTagNo(tvb, offset)) {
            case 0:
                offset = fTimeSpan (tvb, tree, offset, "Time Delay");
                break;
            case 1:
                offset = fUnsignedTag (tvb, tree, offset,
                                       "low-limit: ");
                break;
            case 2:
                offset = fUnsignedTag (tvb, tree, offset,
                                       "high-limit: ");
                break;
            default:
                break;
            }
        }
        break;
        /
    default:
        break;
    }

    /
    offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    return offset;
}
