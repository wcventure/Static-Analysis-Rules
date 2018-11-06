static int decode_block_refinement(MJpegDecodeContext *s, int16_t *block,
                                   uint8_t *last_nnz,
                                   int ac_index, int16_t *quant_matrix,
                                   int ss, int se, int Al, int *EOBRUN)
{
    int code, i = ss, j, sign, val, run;
    int last    = FFMIN(se, *last_nnz);

    OPEN_READER(re, &s->gb);
    if (*EOBRUN) {
        (*EOBRUN)--;
    } else {
        for (; ; i++) {
            UPDATE_CACHE(re, &s->gb);
            GET_VLC(code, re, &s->gb, s->vlcs[2][ac_index].table, 9, 2);

            if (code & 0xF) {
                run = ((unsigned) code) >> 4;
                UPDATE_CACHE(re, &s->gb);
                val = SHOW_UBITS(re, &s->gb, 1);
                LAST_SKIP_BITS(re, &s->gb, 1);
                ZERO_RUN;
                j = s->scantable.permutated[i];
                val--;
                block[j] = ((quant_matrix[j]^val) - val) << Al;
                if (i == se) {
                    if (i > *last_nnz)
                        *last_nnz = i;
                    CLOSE_READER(re, &s->gb);
                    return 0;
                }
            } else {
                run = ((unsigned) code) >> 4;
                if (run == 0xF) {
                    ZERO_RUN;
                } else {
                    val = run;
                    run = (1 << run);
                    if (val) {
                        UPDATE_CACHE(re, &s->gb);
                        run += SHOW_UBITS(re, &s->gb, val);
                        LAST_SKIP_BITS(re, &s->gb, val);
                    }
                    *EOBRUN = run - 1;
                    break;
                }
            }
        }

        if (i > *last_nnz)
            *last_nnz = i;
    }

    for (; i <= last; i++) {
        j = s->scantable.permutated[i];
        if (block[j])
            REFINE_BIT(j)
    }
    CLOSE_READER(re, &s->gb);

    return 0;
}
