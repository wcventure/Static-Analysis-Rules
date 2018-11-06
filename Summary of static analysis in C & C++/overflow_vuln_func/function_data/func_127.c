static guint
fRecipient (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint8  tag_no, tag_info;
    guint32 lvt;

    fTagHeader(tvb, offset, &tag_no, &tag_info, &lvt);
    if (tag_no < 2) {
        if (tag_no == 0) { /
            offset = fObjectIdentifier (tvb, pinfo, tree, offset);
        }
        else {  /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fAddress (tvb, tree, offset);
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
        }
    }
    return offset;
}
