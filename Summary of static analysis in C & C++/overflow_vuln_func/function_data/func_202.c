static guint
fWhoIsRequest  (tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint   val;
    guint8  tag_len;

    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

        switch (tag_no) {
        case 0:
            /
            if (col_get_writable(pinfo->cinfo) && fUnsigned32(tvb, offset+tag_len, lvt, &val))
                col_append_fstr(pinfo->cinfo, COL_INFO, "%d ", val);
            offset = fDevice_Instance (tvb, tree, offset,
                hf_Device_Instance_Range_Low_Limit);
            break;
        case 1:
            /
            if (col_get_writable(pinfo->cinfo) && fUnsigned32(tvb, offset+tag_len, lvt, &val))
                col_append_fstr(pinfo->cinfo, COL_INFO, "%d ", val);
            offset = fDevice_Instance (tvb, tree, offset,
                hf_Device_Instance_Range_High_Limit);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
