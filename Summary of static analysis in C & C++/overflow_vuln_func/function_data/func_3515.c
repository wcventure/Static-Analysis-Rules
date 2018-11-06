static void dirac_unpack_block_motion_data(DiracContext *s)
{
    GetBitContext *gb = &s->gb;
    uint8_t *sbsplit = s->sbsplit;
    int i, x, y, q, p;
    DiracArith arith[8];

    align_get_bits(gb);

    /
    s->sbwidth  = DIVRNDUP(s->source.width,  4*s->plane[0].xbsep);
    s->sbheight = DIVRNDUP(s->source.height, 4*s->plane[0].ybsep);
    s->blwidth  = 4 * s->sbwidth;
    s->blheight = 4 * s->sbheight;

    /
    ff_dirac_init_arith_decoder(arith, gb, svq3_get_ue_golomb(gb));     /
    for (y = 0; y < s->sbheight; y++) {
        for (x = 0; x < s->sbwidth; x++) {
            int split  = dirac_get_arith_uint(arith, CTX_SB_F1, CTX_SB_DATA);
            sbsplit[x] = (split + pred_sbsplit(sbsplit+x, s->sbwidth, x, y)) % 3;
        }
        sbsplit += s->sbwidth;
    }

    /
    ff_dirac_init_arith_decoder(arith, gb, svq3_get_ue_golomb(gb));
    for (i = 0; i < s->num_refs; i++) {
        ff_dirac_init_arith_decoder(arith + 4 + 2 * i, gb, svq3_get_ue_golomb(gb));
        ff_dirac_init_arith_decoder(arith + 5 + 2 * i, gb, svq3_get_ue_golomb(gb));
    }
    for (i = 0; i < 3; i++)
        ff_dirac_init_arith_decoder(arith+1+i, gb, svq3_get_ue_golomb(gb));

    for (y = 0; y < s->sbheight; y++)
        for (x = 0; x < s->sbwidth; x++) {
            int blkcnt = 1 << s->sbsplit[y * s->sbwidth + x];
            int step   = 4 >> s->sbsplit[y * s->sbwidth + x];

            for (q = 0; q < blkcnt; q++)
                for (p = 0; p < blkcnt; p++) {
                    int bx = 4 * x + p*step;
                    int by = 4 * y + q*step;
                    DiracBlock *block = &s->blmotion[by*s->blwidth + bx];
                    decode_block_params(s, arith, block, s->blwidth, bx, by);
                    propagate_block_data(block, s->blwidth, step);
                }
        }
}
