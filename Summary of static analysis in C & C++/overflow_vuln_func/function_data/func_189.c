static guint
fSelectionCriteria (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0, len;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            offset += len;
            continue;
        }

        switch (fTagNo(tvb,offset)) {
        case 0: /
            offset  = fPropertyIdentifier (tvb, pinfo, tree, offset);
            break;
        case 1: /
            offset  = fPropertyArrayIndex (tvb, tree, offset);
            break;
        case 2: /
            offset  = fEnumeratedTag (tvb, tree, offset,
                "relation Specifier: ", BACnetRelationSpecifier);
            break;
        case 3: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fAbstractSyntaxNType (tvb, pinfo, tree, offset);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
