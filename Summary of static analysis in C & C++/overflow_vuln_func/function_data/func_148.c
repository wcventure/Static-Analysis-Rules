static guint
fPropertyValue (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, guint8 tag_info)
{
    guint8  tag_no;
    guint32 lvt;

    if (tag_is_opening(tag_info)) {
        offset += fTagHeaderTree(tvb, tree, offset,
                                 &tag_no, &tag_info, &lvt);
        offset  = fAbstractSyntaxNType (tvb, pinfo, tree, offset);
        if (tvb_length_remaining(tvb, offset) > 0) {
            offset += fTagHeaderTree(tvb, tree, offset,
                                     &tag_no, &tag_info, &lvt);
        }
    } else {
        proto_tree_add_text(tree, tvb, offset, tvb_length(tvb) - offset,
                            "expected Opening Tag!");
        offset = tvb_length(tvb);
    }

    return offset;
}
