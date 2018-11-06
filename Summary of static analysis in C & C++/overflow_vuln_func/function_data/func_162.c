static guint
fLifeSafetyOperationRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, const gchar *label)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    if (label != NULL) {
        tt = proto_tree_add_text (subtree, tvb, offset, 1, "%s", label);
        subtree = proto_item_add_subtree(tt, ett_bacapp_value);
    }

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

        switch (tag_no) {
        case 0: /
            offset = fUnsignedTag (tvb, subtree, offset, "requesting Process Id: ");
            break;
        case 1: /
            offset = fCharacterString (tvb, tree, offset, "requesting Source: ");
            break;
        case 2: /
            offset = fEnumeratedTagSplit (tvb, tree, offset,
                "request: ", BACnetLifeSafetyOperation, 64);
            break;
        case 3: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
