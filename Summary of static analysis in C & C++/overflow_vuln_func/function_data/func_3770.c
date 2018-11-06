static char *choose_pixel_fmts(OutputStream *ost)
{
    if (ost->st->codec->pix_fmt != PIX_FMT_NONE) {
        return av_strdup(av_get_pix_fmt_name(ost->st->codec->pix_fmt));
    } else if (ost->enc->pix_fmts) {
        const enum PixelFormat *p;
        AVIOContext *s = NULL;
        uint8_t *ret;
        int len;

        if (avio_open_dyn_buf(&s) < 0)
            exit_program(1);

        for (p = ost->enc->pix_fmts; *p != PIX_FMT_NONE; p++)
            avio_printf(s, "%s:", av_get_pix_fmt_name(*p));
        len = avio_close_dyn_buf(s, &ret);
        ret[len - 1] = 0;
        return ret;
    } else
        return NULL;
}
