static guint
fDate (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint32     year, month, day, weekday;
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    year = tvb_get_guint8(tvb, offset+tag_len);
    month = tvb_get_guint8(tvb, offset+tag_len+1);
    day = tvb_get_guint8(tvb, offset+tag_len+2);
    weekday = tvb_get_guint8(tvb, offset+tag_len+3);
    if ((year == 255) && (day == 255) && (month == 255) && (weekday == 255)) {
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%sany", label);
    }
    else if (year != 255) {
        year += 1900;
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s%s %d, %d, (Day of Week = %s)",
            label, val_to_str(month,
                months,
                "month (%d) not found"),
            day, year, val_to_str(weekday,
                day_of_week,
                "(%d) not found"));
    } else {
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s%s %d, any year, (Day of Week = %s)",
            label, val_to_str(month, months, "month (%d) not found"),
            day, val_to_str(weekday, day_of_week, "(%d) not found"));
    }
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
