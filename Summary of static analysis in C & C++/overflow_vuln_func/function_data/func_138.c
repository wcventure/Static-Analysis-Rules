static guint
fApplicationTypesEnumeratedSplit (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset,
    const gchar *label, const value_string *src, guint32 split_val)
{
    guint8  tag_no, tag_info;
    guint32 lvt;
    guint   tag_len;

    if (tvb_reported_length_remaining(tvb, offset) > 0) {

        tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (!tag_is_context_specific(tag_info)) {
            switch (tag_no) {
            case 0: /
                offset = fNullTag(tvb, tree, offset, label);
                break;
            case 1: /
                offset = fBooleanTag(tvb, tree, offset, label);
                break;
            case 2: /
                offset = fUnsignedTag(tvb, tree, offset, label);
                break;
            case 3: /
                offset = fSignedTag(tvb, tree, offset, label);
                break;
            case 4: /
                offset = fRealTag(tvb, tree, offset, label);
                break;
            case 5: /
                offset = fDoubleTag(tvb, tree, offset, label);
                break;
            case 6: /
                offset = fOctetString (tvb, tree, offset, label, lvt);
                break;
            case 7: /
                offset = fCharacterString (tvb,tree,offset,label);
                break;
            case 8: /
                offset = fBitStringTagVS (tvb, tree, offset, label, src);
                break;
            case 9: /
                offset = fEnumeratedTagSplit (tvb, tree, offset, label, src, split_val);
                break;
            case 10: /
                offset = fDate (tvb, tree, offset, label);
                break;
            case 11: /
                offset = fTime (tvb, tree, offset, label);
                break;
            case 12: /
                offset = fObjectIdentifier (tvb, pinfo, tree, offset);
                break;
            case 13: /
            case 14:
            case 15:
                proto_tree_add_text(tree, tvb, offset, lvt+tag_len, "%s'reserved for ASHRAE'", label);
                offset += lvt + tag_len;
                break;
            default:
                break;
            }

        }
    }
    return offset;
}
