static guint
fConfirmedServiceAck (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, gint service_choice)
{
    if (tvb_reported_length_remaining(tvb,offset) <= 0)
        return offset;

    switch (service_choice) {
    case 3: /
        offset = fGetAlarmSummaryAck (tvb, pinfo, tree, offset);
        break;
    case 4: /
        offset = fGetEnrollmentSummaryAck (tvb, pinfo, tree, offset);
        break;
    case 6: /
        offset = fAtomicReadFileAck (tvb, pinfo, tree, offset);
        break;
    case 7: /
        offset = fAtomicWriteFileAck (tvb, tree, offset);
        break;
    case 10: /
        offset = fCreateObjectAck (tvb, pinfo, tree, offset);
        break;
    case 12:
        offset = fReadPropertyAck (tvb, pinfo, tree, offset);
        break;
    case 13:
        offset = fReadPropertyConditionalAck (tvb, pinfo, tree, offset);
        break;
    case 14:
        offset = fReadPropertyMultipleAck (tvb, pinfo, tree, offset);
        break;
    case 18:
        offset = fConfirmedPrivateTransferAck(tvb, pinfo, tree, offset);
        break;
    case 21:
        offset = fVtOpenAck (tvb, pinfo, tree, offset);
        break;
    case 23:
        offset = fVtDataAck (tvb, tree, offset);
        break;
    case 24:
        offset = fAuthenticateAck (tvb, pinfo, tree, offset);
        break;
    case 26:
        offset = fReadRangeAck (tvb, pinfo, tree, offset);
        break;
    case 29:
        offset = fGetEventInformationACK (tvb, pinfo, tree, offset);
        break;
    default:
        return offset;
    }
    return offset;
}
