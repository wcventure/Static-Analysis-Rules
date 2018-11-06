static guint
fWeeklySchedule (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       i = 1; /
    proto_tree *subtree = tree;
    proto_item *tt;

    if (propertyArrayIndex > 0) {
        /
        i = propertyArrayIndex;
    }
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) {
            return offset; /
        }
        tt = proto_tree_add_text(tree, tvb, offset, 0, "%s", val_to_str(i++, day_of_week, "day of week (%d) not found"));
        subtree = proto_item_add_subtree(tt, ett_bacapp_value);
        offset = fDailySchedule (tvb, pinfo, subtree, offset);
        if (offset == lastoffset) break;     /
    }
    return offset;
}
