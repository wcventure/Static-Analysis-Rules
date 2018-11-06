static void count_colors(AVCodecContext *avctx, unsigned hits[33],
                         const AVSubtitleRect *r)
{
    DVDSubtitleContext *dvdc = avctx->priv_data;
    unsigned count[256] = { 0 };
    uint32_t *palette = (uint32_t *)r->pict.data[1];
    uint32_t color;
    int x, y, i, j, match, d, best_d, av_uninit(best_j);
    uint8_t *p = r->pict.data[0];

    for (y = 0; y < r->h; y++) {
        for (x = 0; x < r->w; x++)
            count[*(p++)]++;
        p += r->pict.linesize[0] - r->w;
    }
    for (i = 0; i < 256; i++) {
        if (!count[i]) /
            continue;
        color = palette[i];
        /
        match = color < 0x33000000 ? 0 : color < 0xCC000000 ? 1 : 17;
        if (match) {
            best_d = INT_MAX;
            for (j = 0; j < 16; j++) {
                d = color_distance(color & 0xFFFFFF, dvdc->global_palette[j]);
                if (d < best_d) {
                    best_d = d;
                    best_j = j;
                }
            }
            match += best_j;
        }
        hits[match] += count[i];
    }
}
