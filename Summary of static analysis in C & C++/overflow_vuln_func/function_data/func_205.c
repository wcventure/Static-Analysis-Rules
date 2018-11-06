static guint
fConfirmedPrivateTransferError(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset        = 0;
    guint8      tag_no            = 0, tag_info = 0;
    guint32     lvt               = 0;
    proto_tree *subtree           = tree;
    proto_item *tt;

    guint       vendor_identifier = 0;
    guint       service_number    = 0;
    guint8      tag_len           = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {
        /
        lastoffset = offset;
        tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        switch (tag_no) {
        case 0: /
            offset = fContextTaggedError(tvb, pinfo, subtree, offset);
            break;
        case 1: /
            fUnsigned32(tvb, offset+tag_len, lvt, &vendor_identifier);
            if (col_get_writable(pinfo->cinfo))
                col_append_fstr(pinfo->cinfo, COL_INFO, "V=%u ",    vendor_identifier);
            offset = fVendorIdentifier (tvb, pinfo, subtree, offset);
            break;
        case 2: /
            fUnsigned32(tvb, offset+tag_len, lvt, &service_number);
            if (col_get_writable(pinfo->cinfo))
                col_append_fstr(pinfo->cinfo, COL_INFO, "SN=%u ",   service_number);
            offset = fUnsignedTag (tvb, subtree, offset, "service Number: ");
            break;
        case 3: /
            if (tag_is_opening(tag_info)) {
                tt = proto_tree_add_text(subtree, tvb, offset, 1,
                    "error Parameters");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                propertyIdentifier = -1;
                offset += fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
                offset  = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
            } else if (tag_is_closing(tag_info)) {
                offset += fTagHeaderTree (tvb, subtree, offset,
                    &tag_no, &tag_info, &lvt);
                subtree = tree;
            } else {
                /
                return offset;
            }
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
