static int
find_delimiter(tvbuff_t *tvb, int foffset)
{
    int i;
    int length = 0;
    guint16 c_char;

    for (i=0; i < 256; i++) {
        c_char = tvb_get_guint8(tvb, foffset);
        if (c_char == 0x2a || tvb_length_remaining(tvb, foffset)==0) {
            break;
        }
        foffset++;
        length++;
    }
    return length;
}
