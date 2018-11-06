static int
sss_string(tvbuff_t* tvb, int hfinfo, proto_tree *sss_tree, int offset, gboolean little, guint32 length)
{
    int     foffset = offset;
    guint32 str_length;
    char    buffer[1024];
    guint32 i;
    guint16 c_char;
    guint32 length_remaining = 0;

    if (length==0) {
        if (little) {
            str_length = tvb_get_letohl(tvb, foffset);
        } else {
            str_length = tvb_get_ntohl(tvb, foffset);
        }
        foffset += 4;
    } else {
        str_length = length;
    }
    length_remaining = tvb_length_remaining(tvb, foffset);
    if(str_length > (guint)length_remaining || str_length > 1024) {
        proto_tree_add_string(sss_tree, hfinfo, tvb, foffset,
            length_remaining + 4, "<String too long to process>");
        foffset += length_remaining;
        return foffset;
    }
    if(str_length == 0) {
        proto_tree_add_string(sss_tree, hfinfo, tvb, offset, 4, "<Not Specified>");
        return foffset;
    }
    for ( i = 0; i < str_length; i++ ) {
        c_char = tvb_get_guint8(tvb, foffset );
        if (c_char<0x20 || c_char>0x7e) {
            if (c_char != 0x00) {
                c_char = 0x2e;
                buffer[i] = c_char & 0xff;
            } else {
                i--;
                str_length--;
            }
        } else {
            buffer[i] = c_char & 0xff;
        }
        foffset++;
        length_remaining--;

        if(length_remaining==1) {
            i++;
            break;
        }
    }
    buffer[i] = '\0';

    if (length==0) {
        if (little) {
            str_length = tvb_get_letohl(tvb, offset);
        } else {
            str_length = tvb_get_ntohl(tvb, offset);
        }
        offset += 4;
    } else {
        str_length = length;
    }
    proto_tree_add_string(sss_tree, hfinfo, tvb, offset, str_length, buffer);
    return foffset;
}
