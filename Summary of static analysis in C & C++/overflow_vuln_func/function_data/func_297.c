int
dissect_dcerpc_time_t(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_,
                      proto_tree *tree, guint8 *drep,
                      int hfindex, guint32 *pdata)
{
    guint32 data;
    nstime_t tv;

    data = ((drep[0] & DREP_LITTLE_ENDIAN)
            ? tvb_get_letohl(tvb, offset)
            : tvb_get_ntohl(tvb, offset));

    tv.secs = data;
    tv.nsecs = 0;
    if (tree && hfindex != -1) {
        if (data == 0xffffffff) {
            /
            proto_tree_add_time_format_value(tree, hfindex, tvb, offset, 4, &tv, "No time specified");
        } else {
            proto_tree_add_time(tree, hfindex, tvb, offset, 4, &tv);
        }
    }
    if (pdata)
        *pdata = data;

    return offset+4;
}
