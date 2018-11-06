static guint
fWhoHas (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fUnsignedTag (tvb, tree, offset, "device Instance Low Limit: ");
            break;
        case 1: /
            offset = fUnsignedTag (tvb, tree, offset, "device Instance High Limit: ");
            break;
        case 2: /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 3: /
            offset = fCharacterString (tvb,tree,offset, "Object Name: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
