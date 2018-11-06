static guint
fVTCloseError(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8  tag_no = 0, tag_info = 0;
    guint32 lvt = 0;

    if (fTagNo(tvb, offset) == 0) {
        /
        offset = fContextTaggedError(tvb, pinfo, tree,offset);
        if (fTagNo(tvb, offset) == 1) {
            /
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fVtCloseRequest (tvb, pinfo, tree, offset);
            offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
        }
    }
    /
    return offset;
}
