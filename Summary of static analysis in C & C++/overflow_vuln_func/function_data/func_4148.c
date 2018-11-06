static int decompress_p(AVCodecContext *avctx,
                        uint32_t *dst, int linesize,
                        uint32_t *prev, int plinesize)
{
    SCPRContext *s = avctx->priv_data;
    GetByteContext *gb = &s->gb;
    int ret, temp, min, max, x, y, cx = 0, cx1 = 0;
    int backstep = linesize - avctx->width;
    const int cxshift = s->cxshift;

    if (bytestream2_get_byte(gb) == 0)
        return 0;
    bytestream2_skip(gb, 1);
    init_rangecoder(&s->rc, gb);

    ret  = decode_value(s, s->range_model, 256, 1, &min);
    ret |= decode_value(s, s->range_model, 256, 1, &temp);
    min += temp << 8;
    ret |= decode_value(s, s->range_model, 256, 1, &max);
    ret |= decode_value(s, s->range_model, 256, 1, &temp);
    if (ret < 0)
        return ret;

    max += temp << 8;
    memset(s->blocks, 0, sizeof(*s->blocks) * s->nbcount);

    while (min <= max) {
        int fill, count;

        ret  = decode_value(s, s->fill_model,  5,   10, &fill);
        ret |= decode_value(s, s->count_model, 256, 20, &count);
        if (ret < 0)
            return ret;

        while (min < s->nbcount && count-- > 0) {
            s->blocks[min++] = fill;
        }
    }

    for (y = 0; y < s->nby; y++) {
        for (x = 0; x < s->nbx; x++) {
            int sy1 = 0, sy2 = 16, sx1 = 0, sx2 = 16;

            if (s->blocks[y * s->nbx + x] == 0)
                continue;

            if (((s->blocks[y * s->nbx + x] - 1) & 1) > 0) {
                ret  = decode_value(s, s->sxy_model[0], 16, 100, &sx1);
                ret |= decode_value(s, s->sxy_model[1], 16, 100, &sy1);
                ret |= decode_value(s, s->sxy_model[2], 16, 100, &sx2);
                ret |= decode_value(s, s->sxy_model[3], 16, 100, &sy2);
                if (ret < 0)
                    return ret;

                sx2++;
                sy2++;
            }
            if (((s->blocks[y * s->nbx + x] - 1) & 2) > 0) {
                int i, j, by = y * 16, bx = x * 16;
                int mvx, mvy;

                ret  = decode_value(s, s->mv_model[0], 512, 100, &mvx);
                ret |= decode_value(s, s->mv_model[1], 512, 100, &mvy);
                if (ret < 0)
                    return ret;

                mvx -= 256;
                mvy -= 256;

                if (by + mvy + sy1 < 0 || bx + mvx + sx1 < 0 ||
                    by + mvy + sy1 >= avctx->height || bx + mvx + sx1 >= avctx->width)
                    return AVERROR_INVALIDDATA;

                for (i = 0; i < sy2 - sy1 && (by + sy1 + i) < avctx->height && (by + mvy + sy1 + i) < avctx->height; i++) {
                    for (j = 0; j < sx2 - sx1 && (bx + sx1 + j) < avctx->width && (bx + mvx + sx1 + j) < avctx->width; j++) {
                        dst[(by + i + sy1) * linesize + bx + sx1 + j] = prev[(by + mvy + sy1 + i) * plinesize + bx + sx1 + mvx + j];
                    }
                }
            } else {
                int run, r, g, b, z, bx = x * 16 + sx1, by = y * 16 + sy1;
                unsigned clr, ptype = 0;

                for (; by < y * 16 + sy2 && by < avctx->height;) {
                    ret = decode_value(s, s->op_model[ptype], 6, 1000, &ptype);
                    if (ptype == 0) {
                        ret = decode_unit(s, &s->pixel_model[0][cx + cx1], 400, &r);
                        if (ret < 0)
                            return ret;

                        cx1 = (cx << 6) & 0xFC0;
                        cx = r >> cxshift;
                        ret = decode_unit(s, &s->pixel_model[1][cx + cx1], 400, &g);
                        if (ret < 0)
                            return ret;

                        cx1 = (cx << 6) & 0xFC0;
                        cx = g >> cxshift;
                        ret = decode_unit(s, &s->pixel_model[2][cx + cx1], 400, &b);
                        if (ret < 0)
                            return ret;

                        clr = (b << 16) + (g << 8) + r;
                    }
                    if (ptype > 5)
                        return AVERROR_INVALIDDATA;
                    ret = decode_value(s, s->run_model[ptype], 256, 400, &run);
                    if (ret < 0)
                        return ret;

                    switch (ptype) {
                    case 0:
                        while (run-- > 0) {
                            if (by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    case 1:
                        while (run-- > 0) {
                            if (bx == 0) {
                                if (by < 1)
                                    return AVERROR_INVALIDDATA;
                                z = backstep;
                            } else {
                                z = 0;
                            }

                            if (by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            clr = dst[by * linesize + bx - 1 - z];
                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    case 2:
                        while (run-- > 0) {
                            if (by < 1 || by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            clr = dst[(by - 1) * linesize + bx];
                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    case 3:
                        while (run-- > 0) {
                            if (by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            clr = prev[by * plinesize + bx];
                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    case 4:
                        while (run-- > 0) {
                            uint8_t *odst = (uint8_t *)dst;

                            if (by < 1 || by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            if (bx == 0) {
                                z = backstep;
                            } else {
                                z = 0;
                            }

                            r = odst[((by - 1) * linesize + bx) * 4] +
                                odst[(by * linesize + bx - 1 - z) * 4] -
                                odst[((by - 1) * linesize + bx - 1 - z) * 4];
                            g = odst[((by - 1) * linesize + bx) * 4 + 1] +
                                odst[(by * linesize + bx - 1 - z) * 4 + 1] -
                                odst[((by - 1) * linesize + bx - 1 - z) * 4 + 1];
                            b = odst[((by - 1) * linesize + bx) * 4 + 2] +
                                odst[(by * linesize + bx - 1 - z) * 4 + 2] -
                                odst[((by - 1) * linesize + bx - 1 - z) * 4 + 2];
                            clr = ((b & 0xFF) << 16) + ((g & 0xFF) << 8) + (r & 0xFF);
                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    case 5:
                        while (run-- > 0) {
                            if (by < 1 || by >= avctx->height)
                                return AVERROR_INVALIDDATA;

                            if (bx == 0) {
                                z = backstep;
                            } else {
                                z = 0;
                            }

                            clr = dst[(by - 1) * linesize + bx - 1 - z];
                            dst[by * linesize + bx] = clr;
                            bx++;
                            if (bx >= x * 16 + sx2 || bx >= avctx->width) {
                                bx = x * 16 + sx1;
                                by++;
                            }
                        }
                        break;
                    }

                    if (avctx->bits_per_coded_sample == 16) {
                        cx1 = (clr & 0x3F00) >> 2;
                        cx = (clr & 0xFFFFFF) >> 16;
                    } else {
                        cx1 = (clr & 0xFC00) >> 4;
                        cx = (clr & 0xFFFFFF) >> 18;
                    }
                }
            }
        }
    }

    return 0;
}
