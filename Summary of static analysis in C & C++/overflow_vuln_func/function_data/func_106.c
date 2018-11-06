static guint
fEnumeratedTagSplit (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label,
    const value_string *vs, guint32 split_val)
{
    guint32     val = 0;
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    /
    if (fUnsigned32 (tvb, offset+tag_len, lvt, &val)) {
        if (vs)
            ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
                "%s %s", label, val_to_split_str(val, split_val, vs,
                ASHRAE_Reserved_Fmt,Vendor_Proprietary_Fmt));
        else
            ti =proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
                "%s %u", label, val);
    } else {
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s - %u octets (enumeration)", label, lvt);
    }
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+lvt;
}
