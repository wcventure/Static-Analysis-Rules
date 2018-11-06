static guint
fAddListElementRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0, len;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree    = tree;
    proto_item *tt;

    col_set_writable(pinfo->cinfo, FALSE); /

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) {
            offset += len;
            subtree = tree;
            continue;
        }

        switch (tag_no) {
        case 0: /
            offset = fBACnetObjectPropertyReference (tvb, pinfo, subtree, offset);
            break;
        case 3: /
            if (tag_is_opening(tag_info)) {
                tt = proto_tree_add_text(subtree, tvb, offset, 1, "listOfElements");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
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
