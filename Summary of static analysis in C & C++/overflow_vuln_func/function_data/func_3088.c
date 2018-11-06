static int decode_frame(AVCodecContext *avctx, void *data, int *data_size, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    DxaDecContext * const c = avctx->priv_data;
    uint8_t *outptr, *srcptr, *tmpptr;
    unsigned long dsize;
    int i, j, compr;
    int stride;
    int orig_buf_size = buf_size;
    int pc = 0;

    /
    if(buf[0]=='C' && buf[1]=='M' && buf[2]=='A' && buf[3]=='P'){
        int r, g, b;

        buf += 4;
        for(i = 0; i < 256; i++){
            r = *buf++;
            g = *buf++;
            b = *buf++;
            c->pal[i] = 0xFF << 24 | r << 16 | g << 8 | b;
        }
        pc = 1;
        buf_size -= 768+4;
    }

    if(avctx->get_buffer(avctx, &c->pic) < 0){
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return -1;
    }
    memcpy(c->pic.data[1], c->pal, AVPALETTE_SIZE);
    c->pic.palette_has_changed = pc;

    outptr = c->pic.data[0];
    srcptr = c->decomp_buf;
    tmpptr = c->prev.data[0];
    stride = c->pic.linesize[0];

    if(buf[0]=='N' && buf[1]=='U' && buf[2]=='L' && buf[3]=='L')
        compr = -1;
    else
        compr = buf[4];

    dsize = c->dsize;
    if((compr != 4 && compr != -1) && uncompress(c->decomp_buf, &dsize, buf + 9, buf_size - 9) != Z_OK){
        av_log(avctx, AV_LOG_ERROR, "Uncompress failed!\n");
        return -1;
    }
    switch(compr){
    case -1:
        c->pic.key_frame = 0;
        c->pic.pict_type = AV_PICTURE_TYPE_P;
        if(c->prev.data[0])
            memcpy(c->pic.data[0], c->prev.data[0], c->pic.linesize[0] * avctx->height);
        else{ // Should happen only when first frame is 'NULL'
            memset(c->pic.data[0], 0, c->pic.linesize[0] * avctx->height);
            c->pic.key_frame = 1;
            c->pic.pict_type = AV_PICTURE_TYPE_I;
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        c->pic.key_frame = !(compr & 1);
        c->pic.pict_type = (compr & 1) ? AV_PICTURE_TYPE_P : AV_PICTURE_TYPE_I;
        for(j = 0; j < avctx->height; j++){
            if(compr & 1){
                for(i = 0; i < avctx->width; i++)
                    outptr[i] = srcptr[i] ^ tmpptr[i];
                tmpptr += stride;
            }else
                memcpy(outptr, srcptr, avctx->width);
            outptr += stride;
            srcptr += avctx->width;
        }
        break;
    case 12: // ScummVM coding
    case 13:
        c->pic.key_frame = 0;
        c->pic.pict_type = AV_PICTURE_TYPE_P;
        decode_13(avctx, c, c->pic.data[0], srcptr, c->prev.data[0]);
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unknown/unsupported compression type %d\n", buf[4]);
        return -1;
    }

    FFSWAP(AVFrame, c->pic, c->prev);
    if(c->pic.data[0])
        avctx->release_buffer(avctx, &c->pic);

    *data_size = sizeof(AVFrame);
    *(AVFrame*)data = c->prev;

    /
    return orig_buf_size;
}
