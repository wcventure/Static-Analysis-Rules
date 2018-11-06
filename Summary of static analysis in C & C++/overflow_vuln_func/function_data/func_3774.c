static void choose_sample_fmt(AVStream *st, AVCodec *codec)
{
    if (codec && codec->sample_fmts) {
        const enum AVSampleFormat *p = codec->sample_fmts;
        for (; *p != -1; p++) {
            if (*p == st->codec->sample_fmt)
                break;
        }
        if (*p == -1) {
            av_log(NULL, AV_LOG_WARNING,
                   "Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
                   av_get_sample_fmt_name(st->codec->sample_fmt),
                   codec->name,
                   av_get_sample_fmt_name(codec->sample_fmts[0]));
            st->codec->sample_fmt = codec->sample_fmts[0];
        }
    }
}
