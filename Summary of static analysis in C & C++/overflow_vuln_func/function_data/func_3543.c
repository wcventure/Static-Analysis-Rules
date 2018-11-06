static inline int wv_unpack_mono(WavpackFrameContext *s, GetBitContext *gb,
                                 void *dst, const int type)
{
    int i, j, count = 0;
    int last, t;
    int A, S, T;
    int pos                  = s->pos;
    uint32_t crc             = s->sc.crc;
    uint32_t crc_extra_bits  = s->extra_sc.crc;
    int16_t *dst16           = dst;
    int32_t *dst32           = dst;
    float *dstfl             = dst;

    s->one = s->zero = s->zeroes = 0;
    do {
        T = wv_get_value(s, gb, 0, &last);
        S = 0;
        if (last)
            break;
        for (i = 0; i < s->terms; i++) {
            t = s->decorr[i].value;
            if (t > 8) {
                if (t & 1)
                    A =  2U * s->decorr[i].samplesA[0] - s->decorr[i].samplesA[1];
                else
                    A = (int)(3U * s->decorr[i].samplesA[0] - s->decorr[i].samplesA[1]) >> 1;
                s->decorr[i].samplesA[1] = s->decorr[i].samplesA[0];
                j                        = 0;
            } else {
                A = s->decorr[i].samplesA[pos];
                j = (pos + t) & 7;
            }
            if (type != AV_SAMPLE_FMT_S16P)
                S = T + ((s->decorr[i].weightA * (int64_t)A + 512) >> 10);
            else
                S = T + ((s->decorr[i].weightA * A + 512) >> 10);
            if (A && T)
                s->decorr[i].weightA -= ((((T ^ A) >> 30) & 2) - 1) * s->decorr[i].delta;
            s->decorr[i].samplesA[j] = T = S;
        }
        pos = (pos + 1) & 7;
        crc = crc * 3 + S;

        if (type == AV_SAMPLE_FMT_FLTP) {
            *dstfl++ = wv_get_value_float(s, &crc_extra_bits, S);
        } else if (type == AV_SAMPLE_FMT_S32P) {
            *dst32++ = wv_get_value_integer(s, &crc_extra_bits, S);
        } else {
            *dst16++ = wv_get_value_integer(s, &crc_extra_bits, S);
        }
        count++;
    } while (!last && count < s->samples);

    wv_reset_saved_context(s);

    if (last && count < s->samples) {
        int size = av_get_bytes_per_sample(type);
        memset((uint8_t*)dst + count*size, 0, (s->samples-count)*size);
    }

    if (s->avctx->err_recognition & AV_EF_CRCCHECK) {
        int ret = wv_check_crc(s, crc, crc_extra_bits);
        if (ret < 0 && s->avctx->err_recognition & AV_EF_EXPLODE)
            return ret;
    }

    return 0;
}
