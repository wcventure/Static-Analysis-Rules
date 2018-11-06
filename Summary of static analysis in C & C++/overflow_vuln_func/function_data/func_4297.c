static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    DrawBoxContext *s = inlink->dst->priv;
    int plane, x, y, xb = s->x, yb = s->y;
    unsigned char *row[4];

    for (y = FFMAX(yb, 0); y < frame->height && y < (yb + s->h); y++) {
        row[0] = frame->data[0] + y * frame->linesize[0];

        for (plane = 1; plane < 3; plane++)
            row[plane] = frame->data[plane] +
                 frame->linesize[plane] * (y >> s->vsub);

        if (s->invert_color) {
            for (x = FFMAX(xb, 0); x < xb + s->w && x < frame->width; x++)
                if ((y - yb < s->thickness-1) || (yb + s->h - y < s->thickness) ||
                    (x - xb < s->thickness-1) || (xb + s->w - x < s->thickness))
                    row[0][x] = 0xff - row[0][x];
        } else {
            for (x = FFMAX(xb, 0); x < xb + s->w && x < frame->width; x++) {
                double alpha = (double)s->yuv_color[A] / 255;

                if ((y - yb < s->thickness-1) || (yb + s->h - y < s->thickness) ||
                    (x - xb < s->thickness-1) || (xb + s->w - x < s->thickness)) {
                    row[0][x                 ] = (1 - alpha) * row[0][x                 ] + alpha * s->yuv_color[Y];
                    row[1][x >> s->hsub] = (1 - alpha) * row[1][x >> s->hsub] + alpha * s->yuv_color[U];
                    row[2][x >> s->hsub] = (1 - alpha) * row[2][x >> s->hsub] + alpha * s->yuv_color[V];
                }
            }
        }
    }

    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
