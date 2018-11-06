static int copy_cell(Indeo3DecodeContext *ctx, Plane *plane, Cell *cell)
{
    int     h, w, mv_x, mv_y, offset, offset_dst;
    uint8_t *src, *dst;

    /
    offset_dst  = (cell->ypos << 2) * plane->pitch + (cell->xpos << 2);
    dst         = plane->pixels[ctx->buf_sel] + offset_dst;
    mv_y        = cell->mv_ptr[0];
    mv_x        = cell->mv_ptr[1];

    /
    if ((cell->ypos << 2) + mv_y < -1 || (cell->xpos << 2) + mv_x < 0 ||
        ((cell->ypos + cell->height) << 2) + mv_y >= plane->height    ||
        ((cell->xpos + cell->width)  << 2) + mv_x >= plane->width) {
        av_log(ctx->avctx, AV_LOG_ERROR,
               "Motion vectors point out of the frame.\n");
        return AVERROR_INVALIDDATA;
    }

    offset      = offset_dst + mv_y * plane->pitch + mv_x;
    src         = plane->pixels[ctx->buf_sel ^ 1] + offset;

    h = cell->height << 2;

    for (w = cell->width; w > 0;) {
        /
        if (!((cell->xpos << 2) & 15) && w >= 4) {
            for (; w >= 4; src += 16, dst += 16, w -= 4)
                ctx->hdsp.put_no_rnd_pixels_tab[0][0](dst, src, plane->pitch, h);
        }

        /
        if (!((cell->xpos << 2) & 7) && w >= 2) {
            ctx->hdsp.put_no_rnd_pixels_tab[1][0](dst, src, plane->pitch, h);
            w -= 2;
            src += 8;
            dst += 8;
        }

        if (w >= 1) {
            ctx->hdsp.put_no_rnd_pixels_tab[2][0](dst, src, plane->pitch, h);
            w--;
            src += 4;
            dst += 4;
        }
    }

    return 0;
}
