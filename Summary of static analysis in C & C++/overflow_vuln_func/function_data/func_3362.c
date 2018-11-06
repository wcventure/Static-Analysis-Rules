static inline int wv_unpack_stereo(WavpackFrameContext *s, GetBitContext *gb,
                                   void *dst_l, void *dst_r, const int type)
{
    int i, j, count = 0;
    int last, t;
    int A, B, L, L2, R, R2;
    int pos                 = s->pos;
    uint32_t crc            = s->sc.crc;
    uint32_t crc_extra_bits = s->extra_sc.crc;
    int16_t *dst16_l        = dst_l;
    int16_t *dst16_r        = dst_r;
    int32_t *dst32_l        = dst_l;
    int32_t *dst32_r        = dst_r;
    float *dstfl_l          = dst_l;
    float *dstfl_r          = dst_r;

    s->one = s->zero = s->zeroes = 0;
    do {
        L = wv_get_value(s, gb, 0, &last);
        if (last)
            break;
        R = wv_get_value(s, gb, 1, &last);
        if (last)
            break;
        for (i = 0; i < s->terms; i++) {
            t = s->decorr[i].value;
            if (t > 0) {
                if (t > 8) {
                    if (t & 1) {
                        A = 2 * s->decorr[i].samplesA[0] - s->decorr[i].samplesA[1];
                        B = 2 * s->decorr[i].samplesB[0] - s->decorr[i].samplesB[1];
                    } else {
                        A = (3 * s->decorr[i].samplesA[0] - s->decorr[i].samplesA[1]) >> 1;
                        B = (3 * s->decorr[i].samplesB[0] - s->decorr[i].samplesB[1]) >> 1;
                    }
                    s->decorr[i].samplesA[1] = s->decorr[i].samplesA[0];
                    s->decorr[i].samplesB[1] = s->decorr[i].samplesB[0];
                    j                        = 0;
                } else {
                    A = s->decorr[i].samplesA[pos];
                    B = s->decorr[i].samplesB[pos];
                    j = (pos + t) & 7;
                }
                if (type != AV_SAMPLE_FMT_S16P) {
                    L2 = L + ((s->decorr[i].weightA * (int64_t)A + 512) >> 10);
                    R2 = R + ((s->decorr[i].weightB * (int64_t)B + 512) >> 10);
                } else {
                    L2 = L + ((s->decorr[i].weightA * A + 512) >> 10);
                    R2 = R + ((s->decorr[i].weightB * B + 512) >> 10);
                }
                if (A && L)
                    s->decorr[i].weightA -= ((((L ^ A) >> 30) & 2) - 1) * s->decorr[i].delta;
                if (B && R)
                    s->decorr[i].weightB -= ((((R ^ B) >> 30) & 2) - 1) * s->decorr[i].delta;
                s->decorr[i].samplesA[j] = L = L2;
                s->decorr[i].samplesB[j] = R = R2;
            } else if (t == -1) {
                if (type != AV_SAMPLE_FMT_S16P)
                    L2 = L + ((s->decorr[i].weightA * (int64_t)s->decorr[i].samplesA[0] + 512) >> 10);
                else
                    L2 = L + ((s->decorr[i].weightA * s->decorr[i].samplesA[0] + 512) >> 10);
                UPDATE_WEIGHT_CLIP(s->decorr[i].weightA, s->decorr[i].delta, s->decorr[i].samplesA[0], L);
                L = L2;
                if (type != AV_SAMPLE_FMT_S16P)
                    R2 = R + ((s->decorr[i].weightB * (int64_t)L2 + 512) >> 10);
                else
                    R2 = R + ((s->decorr[i].weightB * L2 + 512) >> 10);
                UPDATE_WEIGHT_CLIP(s->decorr[i].weightB, s->decorr[i].delta, L2, R);
                R                        = R2;
                s->decorr[i].samplesA[0] = R;
            } else {
                if (type != AV_SAMPLE_FMT_S16P)
                    R2 = R + ((s->decorr[i].weightB * (int64_t)s->decorr[i].samplesB[0] + 512) >> 10);
                else
                    R2 = R + ((s->decorr[i].weightB * s->decorr[i].samplesB[0] + 512) >> 10);
                UPDATE_WEIGHT_CLIP(s->decorr[i].weightB, s->decorr[i].delta, s->decorr[i].samplesB[0], R);
                R = R2;

                if (t == -3) {
                    R2                       = s->decorr[i].samplesA[0];
                    s->decorr[i].samplesA[0] = R;
                }

                if (type != AV_SAMPLE_FMT_S16P)
                    L2 = L + ((s->decorr[i].weightA * (int64_t)R2 + 512) >> 10);
                else
                    L2 = L + ((s->decorr[i].weightA * R2 + 512) >> 10);
                UPDATE_WEIGHT_CLIP(s->decorr[i].weightA, s->decorr[i].delta, R2, L);
                L                        = L2;
                s->decorr[i].samplesB[0] = L;
            }
        }

        if (type == AV_SAMPLE_FMT_S16P) {
            if (FFABS(L) + FFABS(R) > (1<<19)) {
                av_log(s->avctx, AV_LOG_ERROR, "sample %d %d too large\n", L, R);
                return AVERROR_INVALIDDATA;
            }
        }

        pos = (pos + 1) & 7;
        if (s->joint)
            L += (R -= (L >> 1));
        crc = (crc * 3 + L) * 3 + R;

        if (type == AV_SAMPLE_FMT_FLTP) {
            *dstfl_l++ = wv_get_value_float(s, &crc_extra_bits, L);
            *dstfl_r++ = wv_get_value_float(s, &crc_extra_bits, R);
        } else if (type == AV_SAMPLE_FMT_S32P) {
            *dst32_l++ = wv_get_value_integer(s, &crc_extra_bits, L);
            *dst32_r++ = wv_get_value_integer(s, &crc_extra_bits, R);
        } else {
            *dst16_l++ = wv_get_value_integer(s, &crc_extra_bits, L);
            *dst16_r++ = wv_get_value_integer(s, &crc_extra_bits, R);
        }
        count++;
    } while (!last && count < s->samples);

    wv_reset_saved_context(s);

    if (last && count < s->samples) {
        int size = av_get_bytes_per_sample(type);
        memset((uint8_t*)dst_l + count*size, 0, (s->samples-count)*size);
        memset((uint8_t*)dst_r + count*size, 0, (s->samples-count)*size);
    }

    if ((s->avctx->err_recognition & AV_EF_CRCCHECK) &&
        wv_check_crc(s, crc, crc_extra_bits))
        return AVERROR_INVALIDDATA;

    return 0;
}
