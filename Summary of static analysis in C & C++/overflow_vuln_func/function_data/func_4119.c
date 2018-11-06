int rtjpeg_decode_frame_yuv420(RTJpegContext *c, AVFrame *f,
                               const uint8_t *buf, int buf_size) {
    GetBitContext gb;
    int w = c->w / 16, h = c->h / 16;
    int x, y;
    uint8_t *y1 = f->data[0], *y2 = f->data[0] + 8 * f->linesize[0];
    uint8_t *u = f->data[1], *v = f->data[2];
    init_get_bits(&gb, buf, buf_size * 8);
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            DCTELEM *block = c->block;
            if (get_block(&gb, block, c->scan, c->lquant) > 0)
                c->dsp->idct_put(y1, f->linesize[0], block);
            y1 += 8;
            if (get_block(&gb, block, c->scan, c->lquant) > 0)
                c->dsp->idct_put(y1, f->linesize[0], block);
            y1 += 8;
            if (get_block(&gb, block, c->scan, c->lquant) > 0)
                c->dsp->idct_put(y2, f->linesize[0], block);
            y2 += 8;
            if (get_block(&gb, block, c->scan, c->lquant) > 0)
                c->dsp->idct_put(y2, f->linesize[0], block);
            y2 += 8;
            if (get_block(&gb, block, c->scan, c->cquant) > 0)
                c->dsp->idct_put(u, f->linesize[1], block);
            u += 8;
            if (get_block(&gb, block, c->scan, c->cquant) > 0)
                c->dsp->idct_put(v, f->linesize[2], block);
            v += 8;
        }
        y1 += 2 * 8 * (f->linesize[0] - w);
        y2 += 2 * 8 * (f->linesize[0] - w);
        u += 8 * (f->linesize[1] - w);
        v += 8 * (f->linesize[2] - w);
    }
    return get_bits_count(&gb) / 8;
}
