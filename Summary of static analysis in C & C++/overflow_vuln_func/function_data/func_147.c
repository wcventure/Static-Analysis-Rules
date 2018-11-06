static guint
fAbstractSyntaxNType (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8  tag_no, tag_info;
    guint32 lvt;
    guint   lastoffset = 0, depth = 0;
    char    ar[256];
    guint32 save_object_type = object_type;

    if (propertyIdentifier >= 0) {
        g_snprintf (ar, sizeof(ar), "%s: ",
            val_to_split_str(propertyIdentifier, 512,
                BACnetPropertyIdentifier,
                ASHRAE_Reserved_Fmt,
                Vendor_Proprietary_Fmt));
    } else {
        g_snprintf (ar, sizeof(ar), "Abstract Type: ");
    }
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) { /
            if (depth <= 0) return offset;
        }

        /
        switch (propertyIdentifier) {
        case 2: /
            /
            if (tag_is_context_specific(tag_info)) {
                /
                offset = fActionList (tvb, pinfo, tree,offset);
            } else {
                /
                offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                    BACnetAction);
            }
            break;
        case 30: /
            offset = fAddressBinding (tvb,pinfo,tree,offset);
            break;
        case 54: /
            offset = fLOPR (tvb, pinfo, tree,offset);
            break;
        case 55: /
            fSessionKey (tvb, tree, offset);
            break;
        case 79: /
        case 96: /
            offset = fApplicationTypesEnumeratedSplit (tvb, pinfo, tree, offset, ar,
                BACnetObjectType, 128);
            break;
        case 97: /
            offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                BACnetServicesSupported);
            break;
        case 102: /
            offset = fDestination (tvb, pinfo, tree, offset);
            break;
        case 107: /
            offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                BACnetSegmentation);
            break;
        case 111: /
            offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                BACnetStatusFlags);
            break;
        case 112: /
            offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                BACnetDeviceStatus);
            break;
        case 117: /
            offset = fApplicationTypesEnumerated (tvb, pinfo, tree, offset, ar,
                BACnetEngineeringUnits);
            break;
        case 87:    /
            if (propertyArrayIndex == 0) {
                /
                offset = fApplicationTypes (tvb, pinfo, tree, offset, ar);
            } else {
                offset = fPriorityArray (tvb, pinfo, tree, offset);
            }
            break;
        case 38:    /
            if (object_type < 128) {
                if (propertyArrayIndex == 0) {
                    /
                    offset = fApplicationTypes (tvb, pinfo, tree, offset, ar);
                } else {
                    offset = fSpecialEvent (tvb,pinfo,tree,offset);
                }
            }
            break;
        case 19:  /
        case 60:  /
        case 109: /
        case 132: /
            offset = fDeviceObjectPropertyReference (tvb, pinfo, tree, offset);
            break;
        case 123:   /
            if (object_type < 128) {
                if (propertyArrayIndex == 0) {
                    /
                    offset = fApplicationTypes (tvb, pinfo, tree, offset, ar);
                } else {
                    offset = fWeeklySchedule (tvb, pinfo, tree, offset);
                }
            }
            break;
        case 127:   /
            offset = fClientCOV (tvb, pinfo, tree, offset);
            break;
        case 131:  /
            if ( object_type == 25 )
                offset = fEventLogRecord(tvb, pinfo, tree, offset);
            else if ( object_type == 27 )
                offset = fLogMultipleRecord (tvb, pinfo, tree, offset);
            else
                offset = fLogRecord (tvb, pinfo, tree, offset);
            break;
        case 159: /
        case 165: /
            offset = fDeviceObjectReference (tvb, pinfo, tree, offset);
            break;
        case 196: /
            offset = fRestartReason (tvb, pinfo, tree, offset);
            break;
        case 212: /
        case 214: /
        case 218: /
            offset = fShedLevel (tvb, tree, offset);
            break;
        case 152: /
            offset = fCOVSubscription (tvb, pinfo, tree, offset);
            break;
        case 23: /
            offset = fCalendarEntry(tvb, tree, offset);
            break;
        case 116: /
            offset = fRecipient(tvb, pinfo, tree, offset);
            break;
        case 83: /
            offset = fEventParameter(tvb, pinfo, tree, offset);
            break;
        case 211: /
            offset = fDeviceObjectReference (tvb, pinfo, tree, offset);
            break;
        case 130: /
            offset = fEventTimeStamps(tvb, pinfo, tree, offset);
            break;
        case 197: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetLoggingType);
            break;
        case 36: /
            offset = fApplicationTypesEnumeratedSplit(tvb, pinfo, tree, offset, ar, BACnetEventState, 64);
            break;
        case 103: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetReliability);
            break;
        case 72: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetNotifyType);
            break;
        case 208: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetNodeType);
            break;
        case 231: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetDoorStatus);
            break;
        case 233: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetLockStatus);
            break;
        case 235: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetDoorSecuredStatus);
            break;
        case 158: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetMaintenance);
            break;
        case 92: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetProgramState);
            break;
        case 90: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetProgramRequest);
            break;
        case 100: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetProgramError);
            break;
        case 160: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetLifeSafetyMode);
            break;
        case 163: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetSilencedState);
            break;
        case 161: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetLifeSafetyOperation);
            break;
        case 164: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetLifeSafetyState);
            break;
        case 41: /
            offset = fApplicationTypesEnumerated(tvb, pinfo, tree, offset, ar, BACnetFileAccessMethod);
            break;
        case 185:  /
            offset = fPrescale(tvb, pinfo, tree, offset);
            break;
        case 187:  /
            offset = fScale(tvb, pinfo, tree, offset);
            break;
        case 184: /
            offset = fLoggingRecord(tvb, pinfo, tree, offset);
            break;
        case 228: /
            offset = fDoorMembers(tvb, pinfo, tree, offset);
            break;
        case 181: /
            offset = fObjectPropertyReference(tvb, pinfo, tree, offset);
            break;
        case 78: /
            offset = fObjectPropertyReference(tvb, pinfo, tree, offset);
            break;
        case 234: /
            offset = fSequenceOfEnums(tvb, pinfo, tree, offset, "masked-alarm-value: ", BACnetDoorAlarmState);
            break;
        case 53:    /
            save_object_type = object_type;
            offset = fListOfGroupMembers(tvb, pinfo, tree, offset);
            object_type = save_object_type;
            break;
        case 85:    /
            if ( object_type == 11 )    /
            {
                offset = fReadAccessResult(tvb, pinfo, tree, offset);
                break;
            }
            /
        default:
            if (tag_info) {
                if (tag_is_opening(tag_info)) {
                    ++depth;
                    offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
                } else if (tag_is_closing(tag_info)) {
                    --depth;
                    offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
                } else {
                    offset  = fContextTaggedValue(tvb, tree, offset, ar);
                }
            } else {
                offset = fApplicationTypes (tvb, pinfo, tree, offset, ar);
            }
            break;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;

}
