static av_always_inline void decode_subband_internal(DiracContext *s, SubBand *b, int is_arith)
{
    int cb_x, cb_y, left, right, top, bottom;
    DiracArith c;
    GetBitContext gb;
    int cb_width  = s->codeblock[b->level + (b->orientation != subband_ll)].width;
    int cb_height = s->codeblock[b->level + (b->orientation != subband_ll)].height;
    int blockcnt_one = (cb_width + cb_height) == 2;

    if (!b->length)
        return;

    init_get_bits8(&gb, b->coeff_data, b->length);

    if (is_arith)
        ff_dirac_init_arith_decoder(&c, &gb, b->length);

    top = 0;
    for (cb_y = 0; cb_y < cb_height; cb_y++) {
        bottom = (b->height * (cb_y+1)) / cb_height;
        left = 0;
        for (cb_x = 0; cb_x < cb_width; cb_x++) {
            right = (b->width * (cb_x+1)) / cb_width;
            codeblock(s, b, &gb, &c, left, right, top, bottom, blockcnt_one, is_arith);
            left = right;
        }
        top = bottom;
    }

    if (b->orientation == subband_ll && s->num_refs == 0)
        intra_dc_prediction(b);
}
