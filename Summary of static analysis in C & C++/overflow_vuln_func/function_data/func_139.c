static guint
fShedLevel (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        switch (fTagNo(tvb,offset)) {
        case 0: /
            offset = fUnsignedTag (tvb, tree, offset, "shed percent: ");
            break;
        case 1: /
            offset = fUnsignedTag (tvb, tree, offset, "shed level: ");
            break;
        case 2: /
            offset = fRealTag(tvb, tree, offset, "shed amount: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
