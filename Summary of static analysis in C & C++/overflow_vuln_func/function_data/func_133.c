static guint
fPropertyIdentifier (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8       tag_no, tag_info;
    guint32      lvt;
    guint        tag_len;
    proto_item  *ti;
    proto_tree  *subtree;
    const gchar *label = "Property Identifier";

    propertyIdentifier = 0; /
    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    /
    if (fUnsigned32 (tvb, offset+tag_len, lvt, (guint32 *)&propertyIdentifier)) {
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s: %s (%u)", label,
            val_to_split_str(propertyIdentifier, 512,
                BACnetPropertyIdentifier,
                ASHRAE_Reserved_Fmt,
                Vendor_Proprietary_Fmt), propertyIdentifier);
        if (col_get_writable(pinfo->cinfo))
            col_append_fstr(pinfo->cinfo, COL_INFO, "%s ",
                val_to_split_str(propertyIdentifier, 512,
                    BACnetPropertyIdentifier,
                    ASHRAE_Reserved_Fmt,
                    Vendor_Proprietary_Fmt));
    } else {
        /
        return offset;
    }
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    proto_tree_add_item(subtree, hf_BACnetPropertyIdentifier, tvb,
        offset+tag_len, lvt, ENC_BIG_ENDIAN);

    return offset+tag_len+lvt;
}
