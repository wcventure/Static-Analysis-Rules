static av_cold int rl2_decode_init(AVCodecContext *avctx)
{
    Rl2Context *s = avctx->priv_data;
    int back_size;
    int i;
    s->avctx = avctx;
    avctx->pix_fmt = AV_PIX_FMT_PAL8;
    avcodec_get_frame_defaults(&s->frame);

    /
    if(!avctx->extradata || avctx->extradata_size < EXTRADATA1_SIZE){
        av_log(avctx, AV_LOG_ERROR, "invalid extradata size\n");
        return -1;
    }

    /
    s->video_base = AV_RL16(&avctx->extradata[0]);
    s->clr_count = AV_RL32(&avctx->extradata[2]);

    if(s->video_base >= avctx->width * avctx->height){
        av_log(avctx, AV_LOG_ERROR, "invalid video_base\n");
        return -1;
    }

    /
    for(i=0;i<AVPALETTE_COUNT;i++)
        s->palette[i] = 0xFF << 24 | AV_RB24(&avctx->extradata[6 + i * 3]);

    /
    back_size = avctx->extradata_size - EXTRADATA1_SIZE;

    if(back_size > 0){
        unsigned char* back_frame = av_mallocz(avctx->width*avctx->height);
        if(!back_frame)
            return -1;
        rl2_rle_decode(s,avctx->extradata + EXTRADATA1_SIZE,back_size,
                           back_frame,avctx->width,0);
        s->back_frame = back_frame;
    }
    return 0;
}
