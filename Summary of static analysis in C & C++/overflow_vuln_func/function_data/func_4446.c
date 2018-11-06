// FIXME merge cnt/eob arguments?
static av_always_inline int
decode_coeffs_b_generic(VP56RangeCoder *c, int16_t *coef, int n_coeffs,
                        int is_tx32x32, int is8bitsperpixel, int bpp, unsigned (*cnt)[6][3],
                        unsigned (*eob)[6][2], uint8_t (*p)[6][11],
                        int nnz, const int16_t *scan, const int16_t (*nb)[2],
                        const int16_t *band_counts, const int16_t *qmul)
{
    int i = 0, band = 0, band_left = band_counts[band];
    uint8_t *tp = p[0][nnz];
    uint8_t cache[1024];

    do {
        int val, rc;

        val = vp56_rac_get_prob_branchy(c, tp[0]); // eob
        eob[band][nnz][val]++;
        if (!val)
            break;

skip_eob:
        if (!vp56_rac_get_prob_branchy(c, tp[1])) { // zero
            cnt[band][nnz][0]++;
            if (!--band_left)
                band_left = band_counts[++band];
            cache[scan[i]] = 0;
            nnz            = (1 + cache[nb[i][0]] + cache[nb[i][1]]) >> 1;
            tp             = p[band][nnz];
            if (++i == n_coeffs)
                break;  //invalid input; blocks should end with EOB
            goto skip_eob;
        }

        rc = scan[i];
        if (!vp56_rac_get_prob_branchy(c, tp[2])) { // one
            cnt[band][nnz][1]++;
            val       = 1;
            cache[rc] = 1;
        } else {
            // fill in p[3-10] (model fill) - only once per frame for each pos
            if (!tp[3])
                memcpy(&tp[3], ff_vp9_model_pareto8[tp[2]], 8);

            cnt[band][nnz][2]++;
            if (!vp56_rac_get_prob_branchy(c, tp[3])) { // 2, 3, 4
                if (!vp56_rac_get_prob_branchy(c, tp[4])) {
                    cache[rc] = val = 2;
                } else {
                    val       = 3 + vp56_rac_get_prob(c, tp[5]);
                    cache[rc] = 3;
                }
            } else if (!vp56_rac_get_prob_branchy(c, tp[6])) { // cat1/2
                cache[rc] = 4;
                if (!vp56_rac_get_prob_branchy(c, tp[7])) {
                    val  =  vp56_rac_get_prob(c, 159) + 5;
                } else {
                    val  = (vp56_rac_get_prob(c, 165) << 1) + 7;
                    val +=  vp56_rac_get_prob(c, 145);
                }
            } else { // cat 3-6
                cache[rc] = 5;
                if (!vp56_rac_get_prob_branchy(c, tp[8])) {
                    if (!vp56_rac_get_prob_branchy(c, tp[9])) {
                        val  = 11 + (vp56_rac_get_prob(c, 173) << 2);
                        val +=      (vp56_rac_get_prob(c, 148) << 1);
                        val +=       vp56_rac_get_prob(c, 140);
                    } else {
                        val  = 19 + (vp56_rac_get_prob(c, 176) << 3);
                        val +=      (vp56_rac_get_prob(c, 155) << 2);
                        val +=      (vp56_rac_get_prob(c, 140) << 1);
                        val +=       vp56_rac_get_prob(c, 135);
                    }
                } else if (!vp56_rac_get_prob_branchy(c, tp[10])) {
                    val  = (vp56_rac_get_prob(c, 180) << 4) + 35;
                    val += (vp56_rac_get_prob(c, 157) << 3);
                    val += (vp56_rac_get_prob(c, 141) << 2);
                    val += (vp56_rac_get_prob(c, 134) << 1);
                    val +=  vp56_rac_get_prob(c, 130);
                } else {
                    val = 67;
                    if (!is8bitsperpixel) {
                        if (bpp == 12) {
                            val += vp56_rac_get_prob(c, 255) << 17;
                            val += vp56_rac_get_prob(c, 255) << 16;
                        }
                        val +=  (vp56_rac_get_prob(c, 255) << 15);
                        val +=  (vp56_rac_get_prob(c, 255) << 14);
                    }
                    val += (vp56_rac_get_prob(c, 254) << 13);
                    val += (vp56_rac_get_prob(c, 254) << 12);
                    val += (vp56_rac_get_prob(c, 254) << 11);
                    val += (vp56_rac_get_prob(c, 252) << 10);
                    val += (vp56_rac_get_prob(c, 249) << 9);
                    val += (vp56_rac_get_prob(c, 243) << 8);
                    val += (vp56_rac_get_prob(c, 230) << 7);
                    val += (vp56_rac_get_prob(c, 196) << 6);
                    val += (vp56_rac_get_prob(c, 177) << 5);
                    val += (vp56_rac_get_prob(c, 153) << 4);
                    val += (vp56_rac_get_prob(c, 140) << 3);
                    val += (vp56_rac_get_prob(c, 133) << 2);
                    val += (vp56_rac_get_prob(c, 130) << 1);
                    val +=  vp56_rac_get_prob(c, 129);
                }
            }
        }
#define STORE_COEF(c, i, v) do { \
    if (is8bitsperpixel) { \
        c[i] = v; \
    } else { \
        AV_WN32A(&c[i * 2], v); \
    } \
} while (0)
        if (!--band_left)
            band_left = band_counts[++band];
        if (is_tx32x32)
            STORE_COEF(coef, rc, ((vp8_rac_get(c) ? -val : val) * qmul[!!i]) / 2);
        else
            STORE_COEF(coef, rc, (vp8_rac_get(c) ? -val : val) * qmul[!!i]);
        nnz = (1 + cache[nb[i][0]] + cache[nb[i][1]]) >> 1;
        tp = p[band][nnz];
    } while (++i < n_coeffs);

    return i;
}
