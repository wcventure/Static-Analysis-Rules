static char *choose_pixel_fmts(OutputStream *ost)
{
    if (ost->keep_pix_fmt) {
        if (ost->filter)
            avfilter_graph_set_auto_convert(ost->filter->graph->graph,
                                            AVFILTER_AUTO_CONVERT_NONE);
        if (ost->st->codec->pix_fmt == PIX_FMT_NONE)
            return NULL;
        ost->pix_fmts[0] = ost->st->codec->pix_fmt;
        return ost->pix_fmts;
    }
    if (ost->st->codec->pix_fmt != PIX_FMT_NONE) {
        return av_strdup(av_get_pix_fmt_name(choose_pixel_fmt(ost->st, ost->enc, ost->st->codec->pix_fmt)));
    } else if (ost->enc->pix_fmts) {
        const enum PixelFormat *p;
        AVIOContext *s = NULL;
        uint8_t *ret;
        int len;

        if (avio_open_dyn_buf(&s) < 0)
            exit_program(1);

        p = ost->enc->pix_fmts;
        if (ost->st->codec->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
            if (ost->st->codec->codec_id == CODEC_ID_MJPEG) {
                p = (const enum PixelFormat[]) { PIX_FMT_YUVJ420P, PIX_FMT_YUVJ422P, PIX_FMT_YUV420P, PIX_FMT_YUV422P, PIX_FMT_NONE };
            } else if (ost->st->codec->codec_id == CODEC_ID_LJPEG) {
                p = (const enum PixelFormat[]) { PIX_FMT_YUVJ420P, PIX_FMT_YUVJ422P, PIX_FMT_YUVJ444P, PIX_FMT_YUV420P,
                                                    PIX_FMT_YUV422P, PIX_FMT_YUV444P, PIX_FMT_BGRA, PIX_FMT_NONE };
            }
        }

        for (; *p != PIX_FMT_NONE; p++)
            avio_printf(s, "%s:", av_get_pix_fmt_name(*p));
        len = avio_close_dyn_buf(s, &ret);
        ret[len - 1] = 0;
        return ret;
    } else
        return NULL;
}
