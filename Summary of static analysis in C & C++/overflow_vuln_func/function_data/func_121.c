static guint
fDestination (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    if (tvb_reported_length_remaining(tvb, offset) > 0) {
        offset = fApplicationTypesEnumerated(tvb,pinfo,tree,offset,
                                             "valid Days: ", BACnetDaysOfWeek);
        offset = fTime (tvb,tree,offset,"from time: ");
        offset = fTime (tvb,tree,offset,"to time: ");
        offset = fRecipient (tvb,pinfo,tree,offset);
        offset = fProcessId (tvb,tree,offset);
        offset = fApplicationTypes (tvb,pinfo,tree,offset,
                                    "issue confirmed notifications: ");
        offset = fBitStringTagVS (tvb,tree,offset,
                                  "transitions: ", BACnetEventTransitionBits);
    }
    return offset;
}
