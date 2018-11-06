static guint
fConfirmedServiceRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, gint service_choice)
{
    if (tvb_reported_length_remaining(tvb,offset) <= 0)
        return offset;

    switch (service_choice) {
    case 0: /
        offset = fAcknowledgeAlarmRequest (tvb, pinfo, tree, offset);
        break;
    case 1: /
        offset = fConfirmedCOVNotificationRequest (tvb, pinfo, tree, offset);
        break;
    case 2: /
        offset = fConfirmedEventNotificationRequest (tvb, pinfo, tree, offset);
        break;
    case 3: /
        break;
    case 4: /
        offset = fGetEnrollmentSummaryRequest (tvb, pinfo, tree, offset);
        break;
    case 5: /
        offset = fSubscribeCOVRequest(tvb, pinfo, tree, offset);
        break;
    case 6: /
        offset = fAtomicReadFileRequest(tvb, pinfo, tree, offset);
        break;
    case 7: /
        offset = fAtomicWriteFileRequest(tvb, pinfo, tree, offset);
        break;
    case 8: /
        offset = fAddListElementRequest(tvb, pinfo, tree, offset);
        break;
    case 9: /
        offset = fRemoveListElementRequest(tvb, pinfo, tree, offset);
        break;
    case 10: /
        offset = fCreateObjectRequest(tvb, pinfo, tree, offset);
        break;
    case 11: /
        offset = fDeleteObjectRequest(tvb, pinfo, tree, offset);
        break;
    case 12:
        offset = fReadPropertyRequest(tvb, pinfo, tree, offset);
        break;
    case 13:
        offset = fReadPropertyConditionalRequest(tvb, pinfo, tree, offset);
        break;
    case 14:
        offset = fReadPropertyMultipleRequest(tvb, pinfo, tree, offset);
        break;
    case 15:
        offset = fWritePropertyRequest(tvb, pinfo, tree, offset);
        break;
    case 16:
        offset = fWritePropertyMultipleRequest(tvb, pinfo, tree, offset);
        break;
    case 17:
        offset = fDeviceCommunicationControlRequest(tvb, tree, offset);
        break;
    case 18:
        offset = fConfirmedPrivateTransferRequest(tvb, pinfo, tree, offset);
        break;
    case 19:
        offset = fConfirmedTextMessageRequest(tvb, pinfo, tree, offset);
        break;
    case 20:
        offset = fReinitializeDeviceRequest(tvb, tree, offset);
        break;
    case 21:
        offset = fVtOpenRequest(tvb, pinfo, tree, offset);
        break;
    case 22:
        offset = fVtCloseRequest (tvb, pinfo, tree, offset);
        break;
    case 23:
        offset = fVtDataRequest (tvb, pinfo, tree, offset);
        break;
    case 24:
        offset = fAuthenticateRequest (tvb, tree, offset);
        break;
    case 25:
        offset = fRequestKeyRequest (tvb, pinfo, tree, offset);
        break;
    case 26:
        offset = fReadRangeRequest (tvb, pinfo, tree, offset);
        break;
    case 27:
        offset = fLifeSafetyOperationRequest(tvb, pinfo, tree, offset, NULL);
        break;
    case 28:
        offset = fSubscribeCOVPropertyRequest(tvb, pinfo, tree, offset);
        break;
    case 29:
        offset = fGetEventInformationRequest (tvb, pinfo, tree, offset);
        break;
    default:
        return offset;
    }
    return offset;
}
