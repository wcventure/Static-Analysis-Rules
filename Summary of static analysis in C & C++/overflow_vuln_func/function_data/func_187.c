static guint
fPropertyReference (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset, guint8 tagoffset, guint8 list)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    /
    propertyArrayIndex = -1;
    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
        if (tag_is_closing(tag_info)) { /
            return offset;
        } else if (tag_is_opening(tag_info)) { /
            return offset;
        }
        switch (tag_no-tagoffset) {
        case 0: /
            offset = fPropertyIdentifier (tvb, pinfo, tree, offset);
            break;
        case 1: /
            offset = fPropertyArrayIndex (tvb, tree, offset);
            if (list != 0) break; /
        default:
            lastoffset = offset; /
            break;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
