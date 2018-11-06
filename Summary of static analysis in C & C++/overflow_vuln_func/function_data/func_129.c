static guint
fCOVSubscription (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree;
    proto_item *tt;
    proto_tree *orgtree    = tree;
    guint       itemno     = 1;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info) ) {
            return offset;
        }
        switch (tag_no) {

        case 0: /
                /
                tt = proto_tree_add_text(orgtree, tvb, offset, 1, "Subscription %d",itemno);    /
                itemno = itemno + 1;
                tree = proto_item_add_subtree(tt, ett_bacapp_value);

                tt = proto_tree_add_text(tree, tvb, offset, 1, "Recipient");    /
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt); /
                offset  = fRecipientProcess (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);  /
                subtree = tree; /
            break;
        case 1: /
                tt = proto_tree_add_text(tree, tvb, offset, 1, "Monitored Property Reference");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fBACnetObjectPropertyReference (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                subtree = tree;
            break;
        case 2: /
            offset = fBooleanTag (tvb, tree, offset, "Issue Confirmed Notifications: ");
            break;
        case 3: /
            offset = fUnsignedTag (tvb, tree, offset, "Time Remaining: ");
            break;
        case 4: /
            offset = fRealTag (tvb, tree, offset, "COV Increment: ");
            break;
        default:
            break;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
