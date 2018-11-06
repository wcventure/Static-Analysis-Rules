static guint
fContextTaggedError(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8  tag_info   = 0;
    guint8  parsed_tag = 0;
    guint32 lvt        = 0;

    offset += fTagHeaderTree(tvb, tree, offset, &parsed_tag, &tag_info, &lvt);
    offset  = fError(tvb, pinfo, tree, offset);
    return offset + fTagHeaderTree(tvb, tree, offset, &parsed_tag, &tag_info, &lvt);
}
