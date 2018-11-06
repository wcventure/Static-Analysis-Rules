static int encode_frame(FlacEncodeContext *s)
{
    int ch, count;

    count = count_frame_header(s);

    for (ch = 0; ch < s->channels; ch++)
        count += encode_residual_ch(s, ch);

    count += (8 - (count & 7)) & 7; // byte alignment
    count += 16;                    // CRC-16

    return count >> 3;
}
