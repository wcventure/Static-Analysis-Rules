static int common_init(AVCodecContext *avctx){
    HYuvContext *s = avctx->priv_data;
    int i;

    s->avctx= avctx;
    s->flags= avctx->flags;
        
    dsputil_init(&s->dsp, avctx);
    
    s->width= avctx->width;
    s->height= avctx->height;
    assert(s->width>0 && s->height>0);
    
    for(i=0; i<3; i++){
        s->temp[i]= av_malloc(avctx->width + 16);
    }
    return 0;
}
