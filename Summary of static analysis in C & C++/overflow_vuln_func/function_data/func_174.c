static guint
fGetEnrollmentSummaryRequest (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fEnumeratedTag (tvb, tree, offset,
                "acknowledgment Filter: ", BACnetAcknowledgementFilter);
            break;
        case 1: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fRecipientProcess (tvb, pinfo, tree, offset);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 2: /
            offset  = fEnumeratedTag (tvb, tree, offset,
                "event State Filter: ", BACnetEventStateFilter);
            break;
        case 3: /
            offset  = fEnumeratedTag (tvb, tree, offset,
                "event Type Filter: ", BACnetEventType);
            break;
        case 4: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fUnsignedTag (tvb, tree, offset, "min Priority: ");
            offset  = fUnsignedTag (tvb, tree, offset, "max Priority: ");
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 5: /
            offset  = fUnsignedTag (tvb, tree, offset, "notification Class Filter: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
