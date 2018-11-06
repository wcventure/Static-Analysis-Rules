void ff_mpeg4_pred_ac(MpegEncContext *s, int16_t *block, int n, int dir)
{
    int i;
    int16_t *ac_val, *ac_val1;
    int8_t *const qscale_table = s->current_picture.qscale_table;

    /
    ac_val  = s->ac_val[0][0] + s->block_index[n] * 16;
    ac_val1 = ac_val;
    if (s->ac_pred) {
        if (dir == 0) {
            const int xy = s->mb_x - 1 + s->mb_y * s->mb_stride;
            /
            ac_val -= 16;

            if (s->mb_x == 0 || s->qscale == qscale_table[xy] ||
                n == 1 || n == 3) {
                /
                for (i = 1; i < 8; i++)
                    block[s->idsp.idct_permutation[i << 3]] += ac_val[i];
            } else {
                /
                for (i = 1; i < 8; i++)
                    block[s->idsp.idct_permutation[i << 3]] += ROUNDED_DIV(ac_val[i] * qscale_table[xy], s->qscale);
            }
        } else {
            const int xy = s->mb_x + s->mb_y * s->mb_stride - s->mb_stride;
            /
            ac_val -= 16 * s->block_wrap[n];

            if (s->mb_y == 0 || s->qscale == qscale_table[xy] ||
                n == 2 || n == 3) {
                /
                for (i = 1; i < 8; i++)
                    block[s->idsp.idct_permutation[i]] += ac_val[i + 8];
            } else {
                /
                for (i = 1; i < 8; i++)
                    block[s->idsp.idct_permutation[i]] += ROUNDED_DIV(ac_val[i + 8] * qscale_table[xy], s->qscale);
            }
        }
    }
    /
    for (i = 1; i < 8; i++)
        ac_val1[i] = block[s->idsp.idct_permutation[i << 3]];

    /
    for (i = 1; i < 8; i++)
        ac_val1[8 + i] = block[s->idsp.idct_permutation[i]];
}
