static guint
fVtDataAck (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        switch (fTagNo(tvb,offset)) {
        case 0: /
            offset = fBooleanTag (tvb, tree, offset, "all New Data Accepted: ");
            break;
        case 1: /
            offset = fUnsignedTag (tvb, tree, offset, "accepted Octet Count: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
