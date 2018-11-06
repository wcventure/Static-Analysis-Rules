static void decode_subband(DiracContext *s, GetBitContext *gb, int quant,
                           int slice_x, int slice_y, int bits_end,
                           SubBand *b1, SubBand *b2)
{
    int left   = b1->width  * slice_x    / s->num_x;
    int right  = b1->width  *(slice_x+1) / s->num_x;
    int top    = b1->height * slice_y    / s->num_y;
    int bottom = b1->height *(slice_y+1) / s->num_y;

    int qfactor = qscale_tab[quant & 0x7f];
    int qoffset = qoffset_intra_tab[quant & 0x7f] + 2;

    uint8_t *buf1 =      b1->ibuf + top * b1->stride;
    uint8_t *buf2 = b2 ? b2->ibuf + top * b2->stride: NULL;
    int x, y;
    /
    if (get_bits_count(gb) >= bits_end)
        return;

    if (s->pshift) {
        for (y = top; y < bottom; y++) {
            for (x = left; x < right; x++) {
                PARSE_VALUES(int32_t, x, gb, bits_end, buf1, buf2);
            }
            buf1 += b1->stride;
            if (buf2)
                buf2 += b2->stride;
        }
    }
    else {
        for (y = top; y < bottom; y++) {
            for (x = left; x < right; x++) {
                PARSE_VALUES(int16_t, x, gb, bits_end, buf1, buf2);
            }
            buf1 += b1->stride;
            if (buf2)
                buf2 += b2->stride;
        }
    }
}
