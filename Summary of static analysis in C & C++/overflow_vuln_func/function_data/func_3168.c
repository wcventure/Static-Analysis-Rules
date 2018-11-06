static void sub2video_copy_rect(uint8_t *dst, int dst_linesize, int w, int h,
                                AVSubtitleRect *r)
{
    uint32_t *pal, *dst2;
    uint8_t *src, *src2;
    int x, y;

    if (r->type != SUBTITLE_BITMAP) {
        av_log(NULL, AV_LOG_WARNING, "sub2video: non-bitmap subtitle\n");
        return;
    }
    if (r->x < 0 || r->x + r->w > w || r->y < 0 || r->y + r->h > h) {
        av_log(NULL, AV_LOG_WARNING, "sub2video: rectangle overflowing\n");
        return;
    }

    dst += r->y * dst_linesize + r->x * 4;
    src = r->pict.data[0];
    pal = (uint32_t *)r->pict.data[1];
    for (y = 0; y < r->h; y++) {
        dst2 = (uint32_t *)dst;
        src2 = src;
        for (x = 0; x < r->w; x++)
            *(dst2++) = pal[*(src2++)];
        dst += dst_linesize;
        src += r->pict.linesize[0];
    }
}
