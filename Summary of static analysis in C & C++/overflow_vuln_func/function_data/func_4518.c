static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    TiffContext * const s = avctx->priv_data;
    AVFrame *picture = data;
    AVFrame * const p= (AVFrame*)&s->picture;
    const uint8_t *orig_buf = buf, *end_buf = buf + buf_size;
    int id, le, off, ret;
    int i, j, entries;
    int stride, soff, ssize;
    uint8_t *dst;

    //parse image header
    id = AV_RL16(buf); buf += 2;
    if(id == 0x4949) le = 1;
    else if(id == 0x4D4D) le = 0;
    else{
        av_log(avctx, AV_LOG_ERROR, "TIFF header not found\n");
        return -1;
    }
    s->le = le;
    s->invert = 0;
    s->compr = TIFF_RAW;
    s->fill_order = 0;
    // As TIFF 6.0 specification puts it "An arbitrary but carefully chosen number
    // that further identifies the file as a TIFF file"
    if(tget_short(&buf, le) != 42){
        av_log(avctx, AV_LOG_ERROR, "The answer to life, universe and everything is not correct!\n");
        return -1;
    }
    /
    off = tget_long(&buf, le);
    if(orig_buf + off + 14 >= end_buf){
        av_log(avctx, AV_LOG_ERROR, "IFD offset is greater than image size\n");
        return -1;
    }
    buf = orig_buf + off;
    entries = tget_short(&buf, le);
    for(i = 0; i < entries; i++){
        if(tiff_decode_tag(s, orig_buf, buf, end_buf) < 0)
            return -1;
        buf += 12;
    }
    if(!s->stripdata && !s->stripoff){
        av_log(avctx, AV_LOG_ERROR, "Image data is missing\n");
        return -1;
    }
    /
    if ((ret = init_image(s)) < 0)
        return ret;

    if(s->strips == 1 && !s->stripsize){
        av_log(avctx, AV_LOG_WARNING, "Image data size missing\n");
        s->stripsize = buf_size - s->stripoff;
    }
    stride = p->linesize[0];
    dst = p->data[0];
    for(i = 0; i < s->height; i += s->rps){
        if(s->stripsizes)
            ssize = tget(&s->stripsizes, s->sstype, s->le);
        else
            ssize = s->stripsize;

        if (ssize > buf_size) {
            av_log(avctx, AV_LOG_ERROR, "Buffer size is smaller than strip size\n");
            return -1;
        }

        if(s->stripdata){
            soff = tget(&s->stripdata, s->sot, s->le);
        }else
            soff = s->stripoff;
        if (soff < 0) {
            av_log(avctx, AV_LOG_ERROR, "Invalid stripoff: %d\n", soff);
            return AVERROR(EINVAL);
        }
        if(tiff_unpack_strip(s, dst, stride, orig_buf + soff, ssize, FFMIN(s->rps, s->height - i)) < 0)
            break;
        dst += s->rps * stride;
    }
    if(s->predictor == 2){
        dst = p->data[0];
        soff = s->bpp >> 3;
        ssize = s->width * soff;
        for(i = 0; i < s->height; i++) {
            for(j = soff; j < ssize; j++)
                dst[j] += dst[j - soff];
            dst += stride;
        }
    }

    if(s->invert){
        uint8_t *src;
        int j;

        src = s->picture.data[0];
        for(j = 0; j < s->height; j++){
            for(i = 0; i < s->picture.linesize[0]; i++)
                src[i] = 255 - src[i];
            src += s->picture.linesize[0];
        }
    }
    *picture= *(AVFrame*)&s->picture;
    *data_size = sizeof(AVPicture);

    return buf_size;
}
