static guint
fWeekNDay (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint32     month, weekOfMonth, dayOfWeek;
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    month = tvb_get_guint8(tvb, offset+tag_len);
    weekOfMonth = tvb_get_guint8(tvb, offset+tag_len+1);
    dayOfWeek = tvb_get_guint8(tvb, offset+tag_len+2);
    ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len, "%s %s, %s",
                 val_to_str(month, months, "month (%d) not found"),
                 val_to_str(weekOfMonth, weekofmonth, "week of month (%d) not found"),
                 val_to_str(dayOfWeek, day_of_week, "day of week (%d) not found"));
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
