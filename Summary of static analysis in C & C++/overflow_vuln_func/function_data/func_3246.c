static void draw_char(AVCodecContext *avctx, int c, int a)
{
    XbinContext *s = avctx->priv_data;
    if (s->y > avctx->height - s->font_height)
        return;
    ff_draw_pc_font(s->frame.data[0] + s->y * s->frame.linesize[0] + s->x,
                    s->frame.linesize[0], s->font, s->font_height, c,
                    a & 0x0F, a >> 4);
    s->x += FONT_WIDTH;
    if (s->x >= avctx->width) {
        s->x = 0;
        s->y += s->font_height;
    }
}
