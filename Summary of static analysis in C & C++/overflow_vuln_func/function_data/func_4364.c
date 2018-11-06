static void decode_b(AVCodecContext *ctx, int row, int col,
                     struct VP9Filter *lflvl, ptrdiff_t yoff, ptrdiff_t uvoff,
                     enum BlockLevel bl, enum BlockPartition bp)
{
    VP9Context *s = ctx->priv_data;
    VP9Block *b = s->b;
    enum BlockSize bs = bl * 3 + bp;
    int bytesperpixel = s->bytesperpixel;
    int w4 = bwh_tab[1][bs][0], h4 = bwh_tab[1][bs][1], lvl;
    int emu[2];
    AVFrame *f = s->frames[CUR_FRAME].tf.f;

    s->row = row;
    s->row7 = row & 7;
    s->col = col;
    s->col7 = col & 7;
    s->min_mv.x = -(128 + col * 64);
    s->min_mv.y = -(128 + row * 64);
    s->max_mv.x = 128 + (s->cols - col - w4) * 64;
    s->max_mv.y = 128 + (s->rows - row - h4) * 64;
    if (s->pass < 2) {
        b->bs = bs;
        b->bl = bl;
        b->bp = bp;
        decode_mode(ctx);
        b->uvtx = b->tx - ((s->ss_h && w4 * 2 == (1 << b->tx)) ||
                           (s->ss_v && h4 * 2 == (1 << b->tx)));

        if (!b->skip) {
            int has_coeffs;

            if (bytesperpixel == 1) {
                has_coeffs = decode_coeffs_8bpp(ctx);
            } else {
                has_coeffs = decode_coeffs_16bpp(ctx);
            }
            if (!has_coeffs && b->bs <= BS_8x8 && !b->intra) {
                b->skip = 1;
                memset(&s->above_skip_ctx[col], 1, w4);
                memset(&s->left_skip_ctx[s->row7], 1, h4);
            }
        } else {
            int row7 = s->row7;

#define SPLAT_ZERO_CTX(v, n) \
    switch (n) { \
    case 1:  v = 0;          break; \
    case 2:  AV_ZERO16(&v);  break; \
    case 4:  AV_ZERO32(&v);  break; \
    case 8:  AV_ZERO64(&v);  break; \
    case 16: AV_ZERO128(&v); break; \
    }
#define SPLAT_ZERO_YUV(dir, var, off, n, dir2) \
    do { \
        SPLAT_ZERO_CTX(s->dir##_y_##var[off * 2], n * 2); \
        if (s->ss_##dir2) { \
            SPLAT_ZERO_CTX(s->dir##_uv_##var[0][off], n); \
            SPLAT_ZERO_CTX(s->dir##_uv_##var[1][off], n); \
        } else { \
            SPLAT_ZERO_CTX(s->dir##_uv_##var[0][off * 2], n * 2); \
            SPLAT_ZERO_CTX(s->dir##_uv_##var[1][off * 2], n * 2); \
        } \
    } while (0)

            switch (w4) {
            case 1: SPLAT_ZERO_YUV(above, nnz_ctx, col, 1, h); break;
            case 2: SPLAT_ZERO_YUV(above, nnz_ctx, col, 2, h); break;
            case 4: SPLAT_ZERO_YUV(above, nnz_ctx, col, 4, h); break;
            case 8: SPLAT_ZERO_YUV(above, nnz_ctx, col, 8, h); break;
            }
            switch (h4) {
            case 1: SPLAT_ZERO_YUV(left, nnz_ctx, row7, 1, v); break;
            case 2: SPLAT_ZERO_YUV(left, nnz_ctx, row7, 2, v); break;
            case 4: SPLAT_ZERO_YUV(left, nnz_ctx, row7, 4, v); break;
            case 8: SPLAT_ZERO_YUV(left, nnz_ctx, row7, 8, v); break;
            }
        }
        if (s->pass == 1) {
            s->b++;
            s->block += w4 * h4 * 64 * bytesperpixel;
            s->uvblock[0] += w4 * h4 * 64 * bytesperpixel >> (s->ss_h + s->ss_v);
            s->uvblock[1] += w4 * h4 * 64 * bytesperpixel >> (s->ss_h + s->ss_v);
            s->eob += 4 * w4 * h4;
            s->uveob[0] += 4 * w4 * h4 >> (s->ss_h + s->ss_v);
            s->uveob[1] += 4 * w4 * h4 >> (s->ss_h + s->ss_v);

            return;
        }
    }

    // emulated overhangs if the stride of the target buffer can't hold. This
    // makes it possible to support emu-edge and so on even if we have large block
    // overhangs
    emu[0] = (col + w4) * 8 > f->linesize[0] ||
             (row + h4) > s->rows;
    emu[1] = (col + w4) * 4 > f->linesize[1] ||
             (row + h4) > s->rows;
    if (emu[0]) {
        s->dst[0] = s->tmp_y;
        s->y_stride = 128;
    } else {
        s->dst[0] = f->data[0] + yoff;
        s->y_stride = f->linesize[0];
    }
    if (emu[1]) {
        s->dst[1] = s->tmp_uv[0];
        s->dst[2] = s->tmp_uv[1];
        s->uv_stride = 128;
    } else {
        s->dst[1] = f->data[1] + uvoff;
        s->dst[2] = f->data[2] + uvoff;
        s->uv_stride = f->linesize[1];
    }
    if (b->intra) {
        if (s->bpp > 8) {
            intra_recon_16bpp(ctx, yoff, uvoff);
        } else {
            intra_recon_8bpp(ctx, yoff, uvoff);
        }
    } else {
        if (s->bpp > 8) {
            inter_recon_16bpp(ctx);
        } else {
            inter_recon_8bpp(ctx);
        }
    }
    if (emu[0]) {
        int w = FFMIN(s->cols - col, w4) * 8, h = FFMIN(s->rows - row, h4) * 8, n, o = 0;

        for (n = 0; o < w; n++) {
            int bw = 64 >> n;

            av_assert2(n <= 4);
            if (w & bw) {
                s->dsp.mc[n][0][0][0][0](f->data[0] + yoff + o * bytesperpixel, f->linesize[0],
                                         s->tmp_y + o * bytesperpixel, 128, h, 0, 0);
                o += bw;
            }
        }
    }
    if (emu[1]) {
        int w = FFMIN(s->cols - col, w4) * 8 >> s->ss_h;
        int h = FFMIN(s->rows - row, h4) * 8 >> s->ss_v, n, o = 0;

        for (n = s->ss_h; o < w; n++) {
            int bw = 64 >> n;

            av_assert2(n <= 4);
            if (w & bw) {
                s->dsp.mc[n][0][0][0][0](f->data[1] + uvoff + o * bytesperpixel, f->linesize[1],
                                         s->tmp_uv[0] + o * bytesperpixel, 128, h, 0, 0);
                s->dsp.mc[n][0][0][0][0](f->data[2] + uvoff + o * bytesperpixel, f->linesize[2],
                                         s->tmp_uv[1] + o * bytesperpixel, 128, h, 0, 0);
                o += bw;
            }
        }
    }

    // pick filter level and find edges to apply filter to
    if (s->filter.level &&
        (lvl = s->segmentation.feat[b->seg_id].lflvl[b->intra ? 0 : b->ref[0] + 1]
                                                    [b->mode[3] != ZEROMV]) > 0) {
        int x_end = FFMIN(s->cols - col, w4), y_end = FFMIN(s->rows - row, h4);
        int skip_inter = !b->intra && b->skip, col7 = s->col7, row7 = s->row7;

        setctx_2d(&lflvl->level[row7 * 8 + col7], w4, h4, 8, lvl);
        mask_edges(lflvl->mask[0], 0, 0, row7, col7, x_end, y_end, 0, 0, b->tx, skip_inter);
        if (s->ss_h || s->ss_v)
            mask_edges(lflvl->mask[1], s->ss_h, s->ss_v, row7, col7, x_end, y_end,
                       s->cols & 1 && col + w4 >= s->cols ? s->cols & 7 : 0,
                       s->rows & 1 && row + h4 >= s->rows ? s->rows & 7 : 0,
                       b->uvtx, skip_inter);

        if (!s->filter.lim_lut[lvl]) {
            int sharp = s->filter.sharpness;
            int limit = lvl;

            if (sharp > 0) {
                limit >>= (sharp + 3) >> 2;
                limit = FFMIN(limit, 9 - sharp);
            }
            limit = FFMAX(limit, 1);

            s->filter.lim_lut[lvl] = limit;
            s->filter.mblim_lut[lvl] = 2 * (lvl + 2) + limit;
        }
    }

    if (s->pass == 2) {
        s->b++;
        s->block += w4 * h4 * 64 * bytesperpixel;
        s->uvblock[0] += w4 * h4 * 64 * bytesperpixel >> (s->ss_v + s->ss_h);
        s->uvblock[1] += w4 * h4 * 64 * bytesperpixel >> (s->ss_v + s->ss_h);
        s->eob += 4 * w4 * h4;
        s->uveob[0] += 4 * w4 * h4 >> (s->ss_v + s->ss_h);
        s->uveob[1] += 4 * w4 * h4 >> (s->ss_v + s->ss_h);
    }
}
