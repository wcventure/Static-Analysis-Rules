static guint
fAtomicWriteFileAck (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint tag_no = fTagNo(tvb, offset);
    return fSignedTag (tvb, tree, offset, val_to_str(tag_no, BACnetFileStartOption, "unknown option"));
}
