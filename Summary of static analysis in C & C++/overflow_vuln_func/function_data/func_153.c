static guint
fDailySchedule (tvbuff_t *tvb, packet_info *pinfo, proto_tree *subtree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (tag_is_opening(tag_info) && tag_no == 0) {
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt); /
        while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
            lastoffset = offset;
            fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
            if (tag_is_closing(tag_info)) {
                /
                offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                return offset;
            }

            offset = fTimeValue (tvb, pinfo, subtree, offset);
            if (offset == lastoffset) break;    /
        }
    } else if (tag_no == 0 && lvt == 0) {
        /
        offset += fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    }
    return offset;
}
