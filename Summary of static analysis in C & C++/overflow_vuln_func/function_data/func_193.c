static guint
fReadAccessResult (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0, len;
    guint8      tag_no;
    guint8      tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            offset += len;
            if ((tag_no == 4 || tag_no == 5) && (subtree != tree)) subtree = subtree->parent; /
            continue;
        }

        switch (tag_no) {
        case 0: /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
            break;
        case 1: /
            if (tag_is_opening(tag_info)) {
                tt = proto_tree_add_text(tree, tvb, offset, 1, "listOfResults");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            }
            FAULT;
            break;
        case 2: /
            offset = fPropertyIdentifierValue(tvb, pinfo, subtree, offset, 2);
            break;
        case 5: /
            if (tag_is_opening(tag_info)) {
                tt      = proto_tree_add_text(subtree, tvb, offset, 1, "propertyAccessError");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                /
                offset  = fError(tvb, pinfo, subtree, offset);
                break;
            }
            FAULT;
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
