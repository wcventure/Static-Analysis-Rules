static guint
fConfirmedPrivateTransferRequest(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint       lastoffset = 0, len;
    guint8      tag_no, tag_info;
    guint32     lvt;
    proto_tree *subtree = tree;
    proto_item *tt;
    tvbuff_t   *next_tvb;
    guint       vendor_identifier = 0;
    guint       service_number = 0;

    lastoffset = offset;
    len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    fUnsigned32(tvb, offset+len, lvt, &vendor_identifier);
    if (col_get_writable(pinfo->cinfo))
        col_append_fstr(pinfo->cinfo, COL_INFO, "V=%u ", vendor_identifier);
    offset = fVendorIdentifier (tvb, pinfo, subtree, offset);

    next_tvb = tvb_new_subset_remaining(tvb,offset);
    if (dissector_try_uint(bacapp_dissector_table,
        vendor_identifier, next_tvb, pinfo, tree)) {
        /
        offset += tvb_length(next_tvb);
        return offset;
    }

    /

    /
    while (tvb_reported_length_remaining(tvb, offset) > 0) {
        lastoffset = offset;
        len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) {
            if (tag_no == 2) { /
                offset += len;
                subtree = tree;
                continue;
            } else {
                break; /
            }
        }
        switch (tag_no) {

            /
        case 1: /
            fUnsigned32(tvb, offset+len, lvt, &service_number);
            if (col_get_writable(pinfo->cinfo))
                col_append_fstr(pinfo->cinfo, COL_INFO, "SN=%u ",   service_number);
            offset = fUnsignedTag (tvb, subtree, offset, "service Number: ");
            break;
        case 2: /
            if (tag_is_opening(tag_info)) {
                tt = proto_tree_add_text(subtree, tvb, offset, 1, "service Parameters");
                subtree = proto_item_add_subtree(tt, ett_bacapp_value);
                propertyIdentifier = -1;
                offset = fAbstractSyntaxNType (tvb, pinfo, subtree, offset);
                break;
            }
            FAULT;
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }

    return offset;
}
