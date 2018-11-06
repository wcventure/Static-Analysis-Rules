static guint
fTimeStamp (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8  tag_no = 0, tag_info = 0;
    guint32 lvt    = 0;

    if (tvb_reported_length_remaining(tvb, offset) > 0) {   /
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset = fTime (tvb, tree, offset, label?label:"time: ");
            break;
        case 1: /
            offset = fUnsignedTag (tvb, tree, offset,
                label?label:"sequence number: ");
            break;
        case 2: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fDateTime (tvb, tree, offset, label?label:"date time: ");
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
    }

    return offset;
}
