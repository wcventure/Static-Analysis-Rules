static int apply_color_indexing_transform(WebPContext *s)
{
    ImageContext *img;
    ImageContext *pal;
    int i, x, y;
    uint8_t *p, *pi;

    img = &s->image[IMAGE_ROLE_ARGB];
    pal = &s->image[IMAGE_ROLE_COLOR_INDEXING];

    if (pal->size_reduction > 0) {
        GetBitContext gb_g;
        uint8_t *line;
        int pixel_bits = 8 >> pal->size_reduction;

        line = av_malloc(img->frame->linesize[0]);
        if (!line)
            return AVERROR(ENOMEM);

        for (y = 0; y < img->frame->height; y++) {
            p = GET_PIXEL(img->frame, 0, y);
            memcpy(line, p, img->frame->linesize[0]);
            init_get_bits(&gb_g, line, img->frame->linesize[0] * 8);
            skip_bits(&gb_g, 16);
            i = 0;
            for (x = 0; x < img->frame->width; x++) {
                p    = GET_PIXEL(img->frame, x, y);
                p[2] = get_bits(&gb_g, pixel_bits);
                i++;
                if (i == 1 << pal->size_reduction) {
                    skip_bits(&gb_g, 24);
                    i = 0;
                }
            }
        }
        av_free(line);
    }

    for (y = 0; y < img->frame->height; y++) {
        for (x = 0; x < img->frame->width; x++) {
            p = GET_PIXEL(img->frame, x, y);
            i = p[2];
            if (i >= pal->frame->width) {
                av_log(s->avctx, AV_LOG_ERROR, "invalid palette index %d\n", i);
                return AVERROR_INVALIDDATA;
            }
            pi = GET_PIXEL(pal->frame, i, 0);
            AV_COPY32(p, pi);
        }
    }

    return 0;
}
