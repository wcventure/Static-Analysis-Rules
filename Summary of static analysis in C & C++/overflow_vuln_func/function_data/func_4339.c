static int read_high_coeffs(AVCodecContext *avctx, uint8_t *src, int16_t *dst, int size,
                            int c, int a, int d,
                            int width, ptrdiff_t stride)
{
    PixletContext *ctx = avctx->priv_data;
    GetBitContext *b = &ctx->gbit;
    unsigned cnt1, shbits, rlen, nbits, length, i = 0, j = 0, k;
    int ret, escape, pfx, value, yflag, xflag, flag = 0;
    int64_t state = 3, tmp;

    if ((ret = init_get_bits8(b, src, bytestream2_get_bytes_left(&ctx->gb))) < 0)
      return ret;

    if ((a >= 0) + (a ^ (a >> 31)) - (a >> 31) != 1) {
        nbits = 33 - ff_clz((a >= 0) + (a ^ (a >> 31)) - (a >> 31) - 1);
        if (nbits > 16)
            return AVERROR_INVALIDDATA;
    } else {
        nbits = 1;
    }

    length = 25 - nbits;

    while (i < size) {
        if (state >> 8 != -3) {
            value = ff_clz((state >> 8) + 3) ^ 0x1F;
        } else {
            value = -1;
        }

        cnt1 = get_unary(b, 0, length);

        if (cnt1 >= length) {
            cnt1 = get_bits(b, nbits);
        } else {
            pfx = 14 + ((((uint64_t)(value - 14)) >> 32) & (value - 14));
            if (pfx < 1 || pfx > 25)
                return AVERROR_INVALIDDATA;
            cnt1 *= (1 << pfx) - 1;
            shbits = show_bits(b, pfx);
            if (shbits <= 1) {
                skip_bits(b, pfx - 1);
            } else {
                skip_bits(b, pfx);
                cnt1 += shbits - 1;
            }
        }

        xflag = flag + cnt1;
        yflag = xflag;

        if (flag + cnt1 == 0) {
            value = 0;
        } else {
            xflag &= 1u;
            tmp = (int64_t)c * ((yflag + 1) >> 1) + (c >> 1);
            value = xflag + (tmp ^ -xflag);
        }

        i++;
        dst[j++] = value;
        if (j == width) {
            j = 0;
            dst += stride;
        }
        state += (int64_t)d * (uint64_t)yflag - ((int64_t)(d * (uint64_t)state) >> 8);

        flag = 0;

        if (state * 4ULL > 0xFF || i >= size)
            continue;

        pfx = ((state + 8) >> 5) + (state ? ff_clz(state): 32) - 24;
        escape = av_mod_uintp2(16383, pfx);
        cnt1 = get_unary(b, 0, 8);
        if (cnt1 < 8) {
            if (pfx < 1 || pfx > 25)
                return AVERROR_INVALIDDATA;
            value = show_bits(b, pfx);
            if (value > 1) {
                skip_bits(b, pfx);
                rlen = value + escape * cnt1 - 1;
            } else {
                skip_bits(b, pfx - 1);
                rlen = escape * cnt1;
            }
        } else {
            if (get_bits1(b))
                value = get_bits(b, 16);
            else
                value = get_bits(b, 8);

            rlen = value + 8 * escape;
        }

        if (rlen > 0xFFFF || i + rlen > size)
            return AVERROR_INVALIDDATA;
        i += rlen;

        for (k = 0; k < rlen; k++) {
            dst[j++] = 0;
            if (j == width) {
                j = 0;
                dst += stride;
            }
        }

        state = 0;
        flag = rlen < 0xFFFF ? 1 : 0;
    }

    align_get_bits(b);
    return get_bits_count(b) >> 3;
}
