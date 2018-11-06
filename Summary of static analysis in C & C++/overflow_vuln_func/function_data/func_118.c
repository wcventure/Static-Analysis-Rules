static guint
fCalendarEntry (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint8  tag_no, tag_info;
    guint32 lvt;

    switch (fTagNo(tvb, offset)) {
    case 0: /
        offset = fDate    (tvb, tree, offset, "Date: ");
        break;
    case 1: /
        offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
        offset  = fDateRange (tvb, tree, offset);
        offset += fTagHeaderTree(tvb, tree, offset, &tag_no, &tag_info, &lvt);
        break;
    case 2: /
        offset = fWeekNDay (tvb, tree, offset);
        break;
    default:
        return offset;
    }

    return offset;
}
