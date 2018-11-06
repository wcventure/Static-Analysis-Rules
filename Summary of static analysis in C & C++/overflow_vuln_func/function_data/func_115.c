static guint
fTime (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint32     hour, minute, second, msec, lvt;
    guint8      tag_no, tag_info;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    hour    = tvb_get_guint8(tvb, offset+tag_len);
    minute  = tvb_get_guint8(tvb, offset+tag_len+1);
    second  = tvb_get_guint8(tvb, offset+tag_len+2);
    msec    = tvb_get_guint8(tvb, offset+tag_len+3);
    if ((hour == 255) && (minute == 255) && (second == 255) && (msec == 255))
        ti = proto_tree_add_text(tree, tvb, offset,
            lvt+tag_len, "%sany", label);
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s%d:%02d:%02d.%d %s = %02d:%02d:%02d.%d",
            label,
            hour > 12 ? hour - 12 : hour,
            minute, second, msec,
            hour >= 12 ? "P.M." : "A.M.",
            hour, minute, second, msec);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
