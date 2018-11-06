int
dissect_dcerpc_uint16(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_,
                      proto_tree *tree, guint8 *drep,
                      int hfindex, guint16 *pdata)
{
    guint16 data;

    data = ((drep[0] & DREP_LITTLE_ENDIAN)
            ? tvb_get_letohs(tvb, offset)
            : tvb_get_ntohs(tvb, offset));

    if (tree && hfindex != -1) {
        proto_tree_add_item(tree, hfindex, tvb, offset, 2, DREP_ENC_INTEGER(drep));
    }
    if (pdata)
        *pdata = data;
    return offset + 2;
}