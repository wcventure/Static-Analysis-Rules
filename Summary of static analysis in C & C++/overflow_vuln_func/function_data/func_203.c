static guint
fUnconfirmedServiceRequest  (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, gint service_choice)
{
    if (tvb_reported_length_remaining(tvb,offset) <= 0)
        return offset;

    switch (service_choice) {
    case 0: /
        offset = fIAmRequest  (tvb, pinfo, tree, offset);
        break;
    case 1: /
        offset = fIHaveRequest  (tvb, pinfo, tree, offset);
    break;
    case 2: /
        offset = fUnconfirmedCOVNotificationRequest (tvb, pinfo, tree, offset);
        break;
    case 3: /
        offset = fUnconfirmedEventNotificationRequest (tvb, pinfo, tree, offset);
        break;
    case 4: /
        offset = fUnconfirmedPrivateTransferRequest(tvb, pinfo, tree, offset);
        break;
    case 5: /
        offset = fUnconfirmedTextMessageRequest(tvb, pinfo, tree, offset);
        break;
    case 6: /
        offset = fTimeSynchronizationRequest  (tvb, tree, offset);
        break;
    case 7: /
        offset = fWhoHas (tvb, pinfo, tree, offset);
        break;
    case 8: /
        offset = fWhoIsRequest  (tvb, pinfo, tree, offset);
        break;
    case 9: /
        offset = fUTCTimeSynchronizationRequest  (tvb, tree, offset);
        break;
    default:
        break;
    }
    return offset;
}
