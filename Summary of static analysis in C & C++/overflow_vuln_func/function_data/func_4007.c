void ff_compute_frame_duration(AVFormatContext *s, int *pnum, int *pden, AVStream *st,
                               AVCodecParserContext *pc, AVPacket *pkt)
{
    AVRational codec_framerate = s->iformat ? st->codec->framerate :
                                              av_inv_q(st->codec->time_base);
    int frame_size;

    *pnum = 0;
    *pden = 0;
    switch (st->codec->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        if (st->r_frame_rate.num && !pc) {
            *pnum = st->r_frame_rate.den;
            *pden = st->r_frame_rate.num;
        } else if (st->time_base.num * 1000LL > st->time_base.den) {
            *pnum = st->time_base.num;
            *pden = st->time_base.den;
        } else if (codec_framerate.den * 1000LL > codec_framerate.num) {
            *pnum = codec_framerate.den;
            *pden = codec_framerate.num;

            *pden *= st->codec->ticks_per_frame;
            av_assert0(st->codec->ticks_per_frame);
            if (pc && pc->repeat_pict) {
                av_assert0(s->iformat); // this may be wrong for interlaced encoding but its not used for that case
                if (*pnum > INT_MAX / (1 + pc->repeat_pict))
                    *pden /= 1 + pc->repeat_pict;
                else
                    *pnum *= 1 + pc->repeat_pict;
            }
            /
            if (st->codec->ticks_per_frame > 1 && !pc)
                *pnum = *pden = 0;
        }
        break;
    case AVMEDIA_TYPE_AUDIO:
        frame_size = av_get_audio_frame_duration(st->codec, pkt->size);
        if (frame_size <= 0 || st->codec->sample_rate <= 0)
            break;
        *pnum = frame_size;
        *pden = st->codec->sample_rate;
        break;
    default:
        break;
    }
}
