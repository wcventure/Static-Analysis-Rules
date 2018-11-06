int attribute_align_arg avcodec_open(AVCodecContext *avctx, AVCodec *codec)
{
    int ret= -1;

    /
    if (ff_lockmgr_cb) {
        if ((*ff_lockmgr_cb)(&codec_mutex, AV_LOCK_OBTAIN))
            return -1;
    }

    entangled_thread_counter++;
    if(entangled_thread_counter != 1){
        av_log(avctx, AV_LOG_ERROR, "insufficient thread locking around avcodec_open/close()\n");
        goto end;
    }

    if(avctx->codec || !codec)
        goto end;

    if (codec->priv_data_size > 0) {
        avctx->priv_data = av_mallocz(codec->priv_data_size);
        if (!avctx->priv_data) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
    } else {
        avctx->priv_data = NULL;
    }

    if(avctx->coded_width && avctx->coded_height)
        avcodec_set_dimensions(avctx, avctx->coded_width, avctx->coded_height);
    else if(avctx->width && avctx->height)
        avcodec_set_dimensions(avctx, avctx->width, avctx->height);

#define SANE_NB_CHANNELS 128U
    if (((avctx->coded_width || avctx->coded_height)
        && avcodec_check_dimensions(avctx, avctx->coded_width, avctx->coded_height))
        || avctx->channels > SANE_NB_CHANNELS) {
        av_freep(&avctx->priv_data);
        ret = AVERROR(EINVAL);
        goto end;
    }

    avctx->codec = codec;
    avctx->codec_id = codec->id;
    avctx->frame_number = 0;
    if(avctx->codec->init){
        ret = avctx->codec->init(avctx);
        if (ret < 0) {
            av_freep(&avctx->priv_data);
            avctx->codec= NULL;
            goto end;
        }
    }
    ret=0;
end:
    entangled_thread_counter--;

    /
    if (ff_lockmgr_cb) {
        (*ff_lockmgr_cb)(&codec_mutex, AV_LOCK_RELEASE);
    }
    return ret;
}
