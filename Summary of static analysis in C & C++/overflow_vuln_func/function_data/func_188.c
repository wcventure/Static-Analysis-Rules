static guint
fSpecialEvent (tvbuff_t *tvb, packet_info *pinfo, proto_tree *subtree, guint offset)
{
    guint8 tag_no, tag_info;
    guint32 lvt;
    guint lastoffset = 0, len;
    gboolean closing_found = FALSE;  /

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        /
        if (tag_is_closing(tag_info)) {
            /
            /
            if ( closing_found == TRUE )
                break;
            offset += len;
            closing_found = TRUE;
            continue;
        }

        switch (tag_no) {
        case 0: /
            if (tag_is_opening(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fCalendarEntry (tvb, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
            }
            break;
        case 1: /
            offset = fObjectIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 2: /
            if (tag_is_opening(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fTimeValue (tvb, pinfo, subtree, offset);
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                break;
            }
            FAULT;
            break;
        case 3: /
            offset = fUnsignedTag (tvb, subtree, offset, "event priority: ");
            break;
        default:
            return offset;
        }
        closing_found = FALSE; /
        if (offset == lastoffset) break;     /
    }
    return offset;
}
