static void test2_fill_picture(AVFilterContext *ctx, AVFrame *frame)
{
    TestSourceContext *s = ctx->priv;
    FFDrawColor color;
    unsigned alpha = (uint32_t)s->alpha << 24;

    /
    {
        unsigned i, x = 0, x2;

        x = 0;
        for (i = 1; i < 7; i++) {
            x2 = av_rescale(i, s->w, 6);
            x2 = ff_draw_round_to_sub(&s->draw, 0, 0, x2);
            set_color(s, &color, ((i & 1) ? 0xFF0000 : 0) |
                                 ((i & 2) ? 0x00FF00 : 0) |
                                 ((i & 4) ? 0x0000FF : 0) |
                                 alpha);
            ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                              x, 0, x2 - x, frame->height);
            x = x2;
        }
    }

    /
    /
    if (s->h >= 64) {
        unsigned x, dx, y0, y, g0, g;

        dx = ff_draw_round_to_sub(&s->draw, 0, +1, 1);
        y0 = av_rescale_q(s->pts, s->time_base, av_make_q(2, s->h - 16));
        g0 = av_rescale_q(s->pts, s->time_base, av_make_q(1, 128));
        for (x = 0; x < s->w; x += dx) {
            g = (av_rescale(x, 6 * 256, s->w) + g0) % (6 * 256);
            set_color(s, &color, color_gradient(g) | alpha);
            y = y0 + av_rescale(x, s->h / 2, s->w);
            y %= 2 * (s->h - 16);
            if (y > s->h - 16)
                y = 2 * (s->h - 16) - y;
            y = ff_draw_round_to_sub(&s->draw, 1, 0, y);
            ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                              x, y, dx, 16);
        }
    }

    /
    if (s->w >= 64 && s->h >= 64) {
        int l = (FFMIN(s->w, s->h) - 32) >> 1;
        int steps = FFMAX(4, l >> 5);
        int xc = (s->w >> 2) + (s->w >> 1);
        int yc = (s->h >> 2);
        int cycle = l << 2;
        int pos, xh, yh;
        int c, i;

        for (c = 0; c < 3; c++) {
            set_color(s, &color, (0xBBBBBB ^ (0xFF << (c << 3))) | alpha);
            pos = av_rescale_q(s->pts, s->time_base, av_make_q(64 >> (c << 1), cycle)) % cycle;
            xh = pos < 1 * l ? pos :
                 pos < 2 * l ? l :
                 pos < 3 * l ? 3 * l - pos : 0;
            yh = pos < 1 * l ? 0 :
                 pos < 2 * l ? pos - l :
                 pos < 3 * l ? l :
                               cycle - pos;
            xh -= l >> 1;
            yh -= l >> 1;
            for (i = 1; i <= steps; i++) {
                int x = av_rescale(xh, i, steps) + xc;
                int y = av_rescale(yh, i, steps) + yc;
                x = ff_draw_round_to_sub(&s->draw, 0, -1, x);
                y = ff_draw_round_to_sub(&s->draw, 1, -1, y);
                ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                                  x, y, 8, 8);
            }
        }
    }

    /
    if (s->w >= 64 && s->h >= 64) {
        int l = (FFMIN(s->w, s->h) - 16) >> 2;
        int cycle = l << 3;
        int xc = (s->w >> 2);
        int yc = (s->h >> 2) + (s->h >> 1);
        int xm1 = ff_draw_round_to_sub(&s->draw, 0, -1, xc - 8);
        int xm2 = ff_draw_round_to_sub(&s->draw, 0, +1, xc + 8);
        int ym1 = ff_draw_round_to_sub(&s->draw, 1, -1, yc - 8);
        int ym2 = ff_draw_round_to_sub(&s->draw, 1, +1, yc + 8);
        int size, step, x1, x2, y1, y2;

        size = av_rescale_q(s->pts, s->time_base, av_make_q(4, cycle));
        step = size / l;
        size %= l;
        if (step & 1)
            size = l - size;
        step = (step >> 1) & 3;
        set_color(s, &color, 0xFF808080);
        x1 = ff_draw_round_to_sub(&s->draw, 0, -1, xc - 4 - size);
        x2 = ff_draw_round_to_sub(&s->draw, 0, +1, xc + 4 + size);
        y1 = ff_draw_round_to_sub(&s->draw, 1, -1, yc - 4 - size);
        y2 = ff_draw_round_to_sub(&s->draw, 1, +1, yc + 4 + size);
        if (step == 0 || step == 2)
            ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                              x1, ym1, x2 - x1, ym2 - ym1);
        if (step == 1 || step == 2)
            ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                              xm1, y1, xm2 - xm1, y2 - y1);
        if (step == 3)
            ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                              x1, y1, x2 - x1, y2 - y1);
    }

    /
    {
        unsigned xmin = av_rescale(5, s->w, 8);
        unsigned xmax = av_rescale(7, s->w, 8);
        unsigned ymin = av_rescale(5, s->h, 8);
        unsigned ymax = av_rescale(7, s->h, 8);
        unsigned x, y, i, r;
        uint8_t alpha[256];

        r = s->pts;
        for (y = ymin; y < ymax - 15; y += 16) {
            for (x = xmin; x < xmax - 15; x += 16) {
                if ((x ^ y) & 16)
                    continue;
                for (i = 0; i < 256; i++) {
                    r = r * 1664525 + 1013904223;
                    alpha[i] = r >> 24;
                }
                set_color(s, &color, 0xFF00FF80);
                ff_blend_mask(&s->draw, &color, frame->data, frame->linesize,
                                   frame->width, frame->height,
                                   alpha, 16, 16, 16, 3, 0, x, y);
            }
        }
    }

    /
    if (s->w >= 16 && s->h >= 16) {
        unsigned w = s->w - 8;
        unsigned h = s->h - 8;
        unsigned x = av_rescale_q(s->pts, s->time_base, av_make_q(233, 55 * w)) % (w << 1);
        unsigned y = av_rescale_q(s->pts, s->time_base, av_make_q(233, 89 * h)) % (h << 1);
        if (x > w)
            x = (w << 1) - x;
        if (y > h)
            y = (h << 1) - y;
        x = ff_draw_round_to_sub(&s->draw, 0, -1, x);
        y = ff_draw_round_to_sub(&s->draw, 1, -1, y);
        set_color(s, &color, 0xFF8000FF);
        ff_fill_rectangle(&s->draw, &color, frame->data, frame->linesize,
                          x, y, 8, 8);
    }

    /
    {
        char buf[256];
        unsigned time;

        time = av_rescale_q(s->pts, s->time_base, av_make_q(1, 1000)) % 86400000;
        set_color(s, &color, 0xC0000000);
        ff_blend_rectangle(&s->draw, &color, frame->data, frame->linesize,
                           frame->width, frame->height,
                           2, 2, 100, 36);
        set_color(s, &color, 0xFFFF8000);
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d\n%12"PRIi64,
                 time / 3600000, (time / 60000) % 60, (time / 1000) % 60,
                 time % 1000, s->pts);
        draw_text(s, frame, &color, 4, 4, buf);
    }
}
