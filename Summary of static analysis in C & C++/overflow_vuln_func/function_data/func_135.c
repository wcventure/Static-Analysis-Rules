static guint
fCharacterString (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8      tag_no, tag_info, character_set;
    guint32     lvt, l;
    gsize       inbytesleft, outbytesleft = 512;
    guint       offs, extra = 1;
    guint8     *str_val;
    const char *coding;
    guint8      bf_arr[512], *out = &bf_arr[0];
    proto_item *ti;
    proto_tree *subtree;
    guint       start = offset;

    if (tvb_reported_length_remaining(tvb, offset) > 0) {

        offs = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

        character_set = tvb_get_guint8(tvb, offset+offs);
        /
        if (character_set == 1) {
            extra = 3;
        }
        offset += (offs+extra);
        lvt -= (extra);

        do {
            inbytesleft = l = MIN(lvt, 255);
            /
            str_val = tvb_get_ephemeral_string(tvb, offset, l);
            /
            switch (character_set) {
            case ANSI_X34:
                fConvertXXXtoUTF8(str_val, &inbytesleft, out, &outbytesleft, "UTF-8");
                coding = "UTF-8";
                break;
            case IBM_MS_DBCS:
                out = str_val;
                coding = "IBM MS DBCS";
                break;
            case JIS_C_6226:
                out = str_val;
                coding = "JIS C 6226";
                break;
            case ISO_10646_UCS4:
                fConvertXXXtoUTF8(str_val, &inbytesleft, out, &outbytesleft, "UCS-4BE");
                coding = "ISO 10646 UCS-4";
                break;
            case ISO_10646_UCS2:
                fConvertXXXtoUTF8(str_val, &inbytesleft, out, &outbytesleft, "UCS-2BE");
                coding = "ISO 10646 UCS-2";
                break;
            case ISO_18859_1:
                fConvertXXXtoUTF8(str_val, &inbytesleft, out, &outbytesleft, "ISO8859-1");
                coding = "ISO 8859-1";
                break;
            default:
                out = str_val;
                coding = "unknown";
                break;
            }
            ti = proto_tree_add_text(tree, tvb, offset, l, "%s%s '%s'", label, coding, out);
            lvt    -= l;
            offset += l;
        } while (lvt > 0);

        subtree = proto_item_add_subtree(ti, ett_bacapp_tag);

        fTagHeaderTree (tvb, subtree, start, &tag_no, &tag_info, &lvt);
        proto_tree_add_item(subtree, hf_BACnetCharacterSet, tvb, start+offs, 1, ENC_BIG_ENDIAN);

        if (character_set == 1) {
            proto_tree_add_text(subtree, tvb, start+offs+1, 2, "Code Page: %d", tvb_get_ntohs(tvb, start+offs+1));
        }
        /
    }
    return offset;
}
