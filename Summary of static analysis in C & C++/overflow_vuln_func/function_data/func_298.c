int
dissect_dcerpc_uint64(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_,
                      proto_tree *tree, dcerpc_info *di, guint8 *drep,
                      int hfindex, guint64 *pdata)
{
    guint64 data;

    data = ((drep[0] & DREP_LITTLE_ENDIAN)
            ? tvb_get_letoh64(tvb, offset)
            : tvb_get_ntoh64(tvb, offset));

    if (tree && hfindex != -1) {
        header_field_info *hfinfo;

        /
        hfinfo = proto_registrar_get_nth(hfindex);

        switch (hfinfo->type) {
        case FT_UINT64:
            proto_tree_add_uint64(tree, hfindex, tvb, offset, 8, data);
            break;
        case FT_INT64:
            proto_tree_add_int64(tree, hfindex, tvb, offset, 8, data);
            break;
        default:
            /
            DISSECTOR_ASSERT((di->call_data->flags & DCERPC_IS_NDR64) || (data <= G_MAXUINT32));
            proto_tree_add_uint(tree, hfindex, tvb, offset, 8, (guint32)data);
        }
    }
    if (pdata)
        *pdata = data;
    return offset+8;
}
