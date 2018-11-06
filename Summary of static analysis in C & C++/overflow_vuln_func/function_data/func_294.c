int
dissect_dcerpc_uint8(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_,
                     proto_tree *tree, guint8 *drep,
                     int hfindex, guint8 *pdata)
{
    guint8 data;

    data = tvb_get_guint8(tvb, offset);
    if (tree && hfindex != -1) {
        proto_tree_add_item(tree, hfindex, tvb, offset, 1, DREP_ENC_INTEGER(drep));
    }
    if (pdata)
        *pdata = data;
    return offset + 1;
}
