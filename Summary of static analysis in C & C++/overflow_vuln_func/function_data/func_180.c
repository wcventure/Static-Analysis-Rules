static guint
fReinitializeDeviceRequest(tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fEnumeratedTag (tvb, tree, offset,
                "reinitialized State Of Device: ",
                BACnetReinitializedStateOfDevice);
            break;
        case 1: /
            offset = fCharacterString (tvb, tree, offset, "Password: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
