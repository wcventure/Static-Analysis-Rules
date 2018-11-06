static int read_diff_float_data(ALSDecContext *ctx, unsigned int ra_frame) {
    AVCodecContext *avctx   = ctx->avctx;
    GetBitContext *gb       = &ctx->gb;
    SoftFloat_IEEE754 *acf  = ctx->acf;
    int *shift_value        = ctx->shift_value;
    int *last_shift_value   = ctx->last_shift_value;
    int *last_acf_mantissa  = ctx->last_acf_mantissa;
    int **raw_mantissa      = ctx->raw_mantissa;
    int *nbits              = ctx->nbits;
    unsigned char *larray   = ctx->larray;
    int frame_length        = ctx->cur_frame_length;
    SoftFloat_IEEE754 scale = av_int2sf_ieee754(0x1u, 23);
    unsigned int partA_flag;
    unsigned int highest_byte;
    unsigned int shift_amp;
    uint32_t tmp_32;
    int use_acf;
    int nchars;
    int i;
    int c;
    long k;
    long nbits_aligned;
    unsigned long acc;
    unsigned long j;
    uint32_t sign;
    uint32_t e;
    uint32_t mantissa;

    skip_bits_long(gb, 32); //num_bytes_diff_float
    use_acf = get_bits1(gb);

    if (ra_frame) {
        memset(last_acf_mantissa, 0, avctx->channels * sizeof(*last_acf_mantissa));
        memset(last_shift_value,  0, avctx->channels * sizeof(*last_shift_value) );
        ff_mlz_flush_dict(ctx->mlz);
    }

    for (c = 0; c < avctx->channels; ++c) {
        if (use_acf) {
            //acf_flag
            if (get_bits1(gb)) {
                tmp_32 = get_bits(gb, 23);
                last_acf_mantissa[c] = tmp_32;
            } else {
                tmp_32 = last_acf_mantissa[c];
            }
            acf[c] = av_bits2sf_ieee754(tmp_32);
        } else {
            acf[c] = FLOAT_1;
        }

        highest_byte = get_bits(gb, 2);
        partA_flag   = get_bits1(gb);
        shift_amp    = get_bits1(gb);

        if (shift_amp) {
            shift_value[c] = get_bits(gb, 8);
            last_shift_value[c] = shift_value[c];
        } else {
            shift_value[c] = last_shift_value[c];
        }

        if (partA_flag) {
            if (!get_bits1(gb)) { //uncompressed
                for (i = 0; i < frame_length; ++i) {
                    if (ctx->raw_samples[c][i] == 0) {
                        ctx->raw_mantissa[c][i] = get_bits_long(gb, 32);
                    }
                }
            } else { //compressed
                nchars = 0;
                for (i = 0; i < frame_length; ++i) {
                    if (ctx->raw_samples[c][i] == 0) {
                        nchars += 4;
                    }
                }

                tmp_32 = ff_mlz_decompression(ctx->mlz, gb, nchars, larray);
                if(tmp_32 != nchars) {
                    av_log(ctx->avctx, AV_LOG_ERROR, "Error in MLZ decompression (%d, %d).\n", tmp_32, nchars);
                    return AVERROR_INVALIDDATA;
                }

                for (i = 0; i < frame_length; ++i) {
                    ctx->raw_mantissa[c][i] = AV_RB32(larray);
                }
            }
        }

        //decode part B
        if (highest_byte) {
            for (i = 0; i < frame_length; ++i) {
                if (ctx->raw_samples[c][i] != 0) {
                    //The following logic is taken from Tabel 14.45 and 14.46 from the ISO spec
                    if (av_cmp_sf_ieee754(acf[c], FLOAT_1)) {
                        nbits[i] = 23 - av_log2(abs(ctx->raw_samples[c][i]));
                    } else {
                        nbits[i] = 23;
                    }
                    nbits[i] = FFMIN(nbits[i], highest_byte*8);
                }
            }

            if (!get_bits1(gb)) { //uncompressed
                for (i = 0; i < frame_length; ++i) {
                    if (ctx->raw_samples[c][i] != 0) {
                        raw_mantissa[c][i] = get_bits(gb, nbits[i]);
                    }
                }
            } else { //compressed
                nchars = 0;
                for (i = 0; i < frame_length; ++i) {
                    if (ctx->raw_samples[c][i]) {
                        nchars += (int) nbits[i] / 8;
                        if (nbits[i] & 7) {
                            ++nchars;
                        }
                    }
                }

                tmp_32 = ff_mlz_decompression(ctx->mlz, gb, nchars, larray);
                if(tmp_32 != nchars) {
                    av_log(ctx->avctx, AV_LOG_ERROR, "Error in MLZ decompression (%d, %d).\n", tmp_32, nchars);
                    return AVERROR_INVALIDDATA;
                }

                j = 0;
                for (i = 0; i < frame_length; ++i) {
                    if (ctx->raw_samples[c][i]) {
                        if (nbits[i] & 7) {
                            nbits_aligned = 8 * ((unsigned int)(nbits[i] / 8) + 1);
                        } else {
                            nbits_aligned = nbits[i];
                        }
                        acc = 0;
                        for (k = 0; k < nbits_aligned/8; ++k) {
                            acc = (acc << 8) + larray[j++];
                        }
                        acc >>= (nbits_aligned - nbits[i]);
                        raw_mantissa[c][i] = acc;
                    }
                }
            }
        }

        for (i = 0; i < frame_length; ++i) {
            SoftFloat_IEEE754 pcm_sf = av_int2sf_ieee754(ctx->raw_samples[c][i], 0);
            pcm_sf = av_div_sf_ieee754(pcm_sf, scale);

            if (ctx->raw_samples[c][i] != 0) {
                if (!av_cmp_sf_ieee754(acf[c], FLOAT_1)) {
                    pcm_sf = multiply(acf[c], pcm_sf);
                }

                sign = pcm_sf.sign;
                e = pcm_sf.exp;
                mantissa = (pcm_sf.mant | 0x800000) + raw_mantissa[c][i];

                while(mantissa >= 0x1000000) {
                    e++;
                    mantissa >>= 1;
                }

                if (mantissa) e += (shift_value[c] - 127);
                mantissa &= 0x007fffffUL;

                tmp_32 = (sign << 31) | ((e + EXP_BIAS) << 23) | (mantissa);
                ctx->raw_samples[c][i] = tmp_32;
            } else {
                ctx->raw_samples[c][i] = raw_mantissa[c][i] & 0x007fffffUL;
            }
        }
        align_get_bits(gb);
    }
    return 0;
}
