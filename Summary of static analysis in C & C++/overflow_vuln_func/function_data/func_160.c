static guint
fConfirmedTextMessageRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {

        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 1: /
            switch (fTagNo(tvb, offset)) {
            case 0: /
                offset = fUnsignedTag (tvb, tree, offset, "message Class: ");
                break;
            case 1: /
                offset = fCharacterString (tvb, tree, offset, "message Class: ");
                break;
            }
            break;
        case 2: /
            offset = fEnumeratedTag (tvb, tree, offset, "message Priority: ",
                BACnetMessagePriority);
            break;
        case 3: /
            offset = fCharacterString (tvb, tree, offset, "message: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
