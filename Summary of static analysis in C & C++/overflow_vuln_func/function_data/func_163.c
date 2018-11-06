static guint
fBACnetPropertyStates(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, guint offset)
{
    guint8       tag_no, tag_info;
    guint32      lvt;
    const gchar* label = NULL;

    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    label = ep_strdup_printf( "%s: ", val_to_str_const( tag_no, VALS(BACnetPropertyStates), "unknown-"+tag_no ));

    switch (tag_no) {
    case 0:
        offset = fBooleanTag (tvb, tree, offset, label);
        break;
    case 11:
        offset = fUnsignedTag(tvb, tree, offset, label);
        break;
    default:
        if ( (tag_no > BACnetPropertyStatesEnums_Size) ||
            VALS(BACnetPropertyStatesEnums[tag_no].valstr) == NULL)
        {
            offset = fEnumeratedTag(tvb, tree, offset, "Unknown State: ", NULL);
            /
        }
        else
        {
            offset = fEnumeratedTagSplit(tvb, tree, offset, label,
                    VALS(BACnetPropertyStatesEnums[tag_no].valstr), 64);
        }
        break;
    }
    return offset;
}
