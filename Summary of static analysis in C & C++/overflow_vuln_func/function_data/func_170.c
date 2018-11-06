static guint
fLogMultipleRecord (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint   lastoffset = 0;
    guint8  tag_no, tag_info;
    guint32 lvt;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;
        switch (fTagNo(tvb, offset)) {
        case 0: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            offset  = fDate (tvb,tree,offset,"Date: ");
            offset  = fTime (tvb,tree,offset,"Time: ");
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        case 1: /
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            switch (fTagNo(tvb, offset)) {
            case 0: /    /
                offset = fBitStringTagVS(tvb, tree, offset, "log status:", BACnetLogStatus);
                break;
            case 1: /
                offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                while ((tvb_reported_length_remaining(tvb, offset) > 0) && (offset != lastoffset)) {  /
                    lastoffset = offset;
                    fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
                    if (tag_is_closing(tag_info)) {
                        lastoffset = offset;
                        break;
                    }
                    switch (tag_no) {
                    case 0:
                        offset = fBooleanTag (tvb, tree, offset, "boolean-value: ");
                        break;
                    case 1:
                        offset = fRealTag (tvb, tree, offset, "real value: ");
                        break;
                    case 2:
                        offset = fUnsignedTag (tvb, tree, offset, "enum value: ");
                        break;
                    case 3:
                        offset = fUnsignedTag (tvb, tree, offset, "unsigned value: ");
                        break;
                    case 4:
                        offset = fSignedTag (tvb, tree, offset, "signed value: ");
                        break;
                    case 5:
                        offset = fBitStringTag (tvb, tree, offset, "bitstring value: ");
                        break;
                    case 6:
                        offset = fNullTag(tvb, tree, offset, "null value: ");
                        break;
                    case 7:
                        offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                        offset  = fError (tvb, pinfo, tree, offset);
                        offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                        break;
                    case 8: /
                        offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                        offset  = fAbstractSyntaxNType (tvb, pinfo, tree, offset);
                        offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                        break;
                    default:
                        return offset;
                    }
                }
                offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
                break;
            case 2:
                offset = fRealTag (tvb, tree, offset, "time-change: ");
                break;
            default:
                return offset;
            }
            offset += fTagHeaderTree (tvb, tree, offset, &tag_no, &tag_info, &lvt);
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
