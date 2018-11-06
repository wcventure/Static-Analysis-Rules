static void compute_frame_duration(int *pnum, int *pden, AVStream *st, 
                                   AVCodecParserContext *pc, AVPacket *pkt)
{
    int frame_size;

    *pnum = 0;
    *pden = 0;
    switch(st->codec.codec_type) {
    case CODEC_TYPE_VIDEO:
        if(st->time_base.num*1000 > st->time_base.den){
            *pnum = st->time_base.num;
            *pden = st->time_base.den;
        }else if(st->codec.time_base.num*1000 > st->codec.time_base.den){
            *pnum = st->codec.time_base.num;
            *pden = st->codec.time_base.den;
            if (pc && pc->repeat_pict) {
                *pden *= 2;
                *pnum = (*pnum) * (2 + pc->repeat_pict);
            }
        }
        break;
    case CODEC_TYPE_AUDIO:
        frame_size = get_audio_frame_size(&st->codec, pkt->size);
        if (frame_size < 0)
            break;
        *pnum = frame_size;
        *pden = st->codec.sample_rate;
        break;
    default:
        break;
    }
}
