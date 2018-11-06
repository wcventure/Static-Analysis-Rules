static guint
fPropertyIdentifierValue (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, guint8 tagoffset)
{
    guint   lastoffset = offset;
    guint8  tag_no, tag_info;
    guint32 lvt;

    offset = fPropertyReference(tvb, pinfo, tree, offset, tagoffset, 0);
    if (offset > lastoffset) {
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_no == tagoffset+2) {  /
            offset = fPropertyValue (tvb, pinfo, tree, offset, tag_info);
        }
    }
    return offset;
}
