static guint
fCreateObjectError(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fContextTaggedError(tvb, pinfo, tree, offset);
            break;
        case 1: /
            offset = fUnsignedTag (tvb,tree,offset,"first failed element number: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
