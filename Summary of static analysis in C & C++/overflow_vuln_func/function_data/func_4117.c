static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    const uint8_t *buf_end = avpkt->data + avpkt->size;
    TargaContext * const s = avctx->priv_data;
    AVFrame *picture = data;
    AVFrame * const p= (AVFrame*)&s->picture;
    uint8_t *dst;
    int stride;
    int idlen, pal, compr, y, w, h, bpp, flags;
    int first_clr, colors, csize;

    /
    CHECK_BUFFER_SIZE(buf, buf_end, 18, "header");
    idlen = *buf++;
    pal = *buf++;
    compr = *buf++;
    first_clr = bytestream_get_le16(&buf);
    colors = bytestream_get_le16(&buf);
    csize = *buf++;
    if (!pal && (first_clr || colors || csize)) {
        av_log(avctx, AV_LOG_WARNING, "File without colormap has colormap information set.\n");
        // specification says we should ignore those value in this case
        first_clr = colors = csize = 0;
    }
    buf += 2; /
    y = bytestream_get_le16(&buf);
    w = bytestream_get_le16(&buf);
    h = bytestream_get_le16(&buf);
    bpp = *buf++;
    flags = *buf++;
    //skip identifier if any
    CHECK_BUFFER_SIZE(buf, buf_end, idlen, "identifiers");
    buf += idlen;
    s->bpp = bpp;
    s->width = w;
    s->height = h;
    switch(s->bpp){
    case 8:
        avctx->pix_fmt = ((compr & (~TGA_RLE)) == TGA_BW) ? PIX_FMT_GRAY8 : PIX_FMT_PAL8;
        break;
    case 15:
        avctx->pix_fmt = PIX_FMT_RGB555;
        break;
    case 16:
        avctx->pix_fmt = PIX_FMT_RGB555;
        break;
    case 24:
        avctx->pix_fmt = PIX_FMT_BGR24;
        break;
    case 32:
        avctx->pix_fmt = PIX_FMT_RGB32;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Bit depth %i is not supported\n", s->bpp);
        return -1;
    }

    if(s->picture.data[0])
        avctx->release_buffer(avctx, &s->picture);

    if(av_image_check_size(w, h, 0, avctx))
        return -1;
    if(w != avctx->width || h != avctx->height)
        avcodec_set_dimensions(avctx, w, h);
    if(avctx->get_buffer(avctx, p) < 0){
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return -1;
    }
    if(flags & 0x20){
        dst = p->data[0];
        stride = p->linesize[0];
    }else{ //image is upside-down
        dst = p->data[0] + p->linesize[0] * (h - 1);
        stride = -p->linesize[0];
    }

    if(colors){
        size_t pal_size;
        if((colors + first_clr) > 256){
            av_log(avctx, AV_LOG_ERROR, "Incorrect palette: %i colors with offset %i\n", colors, first_clr);
            return -1;
        }
        if(csize != 24){
            av_log(avctx, AV_LOG_ERROR, "Palette entry size %i bits is not supported\n", csize);
            return -1;
        }
        pal_size = colors * ((csize + 1) >> 3);
        CHECK_BUFFER_SIZE(buf, buf_end, pal_size, "color table");
        if(avctx->pix_fmt != PIX_FMT_PAL8)//should not occur but skip palette anyway
            buf += pal_size;
        else{
            int t;
            int32_t *pal = ((int32_t*)p->data[1]) + first_clr;
            for(t = 0; t < colors; t++){
                *pal++ = (0xff<<24) | bytestream_get_le24(&buf);
            }
            p->palette_has_changed = 1;
        }
    }
    if((compr & (~TGA_RLE)) == TGA_NODATA)
        memset(p->data[0], 0, p->linesize[0] * s->height);
    else{
        if(compr & TGA_RLE){
            int res = targa_decode_rle(avctx, s, buf, buf_end - buf, dst, avctx->width, avctx->height, stride, bpp);
            if (res < 0)
                return -1;
            buf += res;
        }else{
            size_t img_size = s->width * ((s->bpp + 1) >> 3);
            CHECK_BUFFER_SIZE(buf, buf_end, img_size, "image data");
            for(y = 0; y < s->height; y++){
#if HAVE_BIGENDIAN
                int x;
                if((s->bpp + 1) >> 3 == 2){
                    uint16_t *dst16 = (uint16_t*)dst;
                    for(x = 0; x < s->width; x++)
                        dst16[x] = AV_RL16(buf + x * 2);
                }else if((s->bpp + 1) >> 3 == 4){
                    uint32_t *dst32 = (uint32_t*)dst;
                    for(x = 0; x < s->width; x++)
                        dst32[x] = AV_RL32(buf + x * 4);
                }else
#endif
                    memcpy(dst, buf, img_size);

                dst += stride;
                buf += img_size;
            }
        }
    }
    if(flags & 0x10){ // right-to-left, needs horizontal flip
        int x;
        for(y = 0; y < s->height; y++){
            void *line = &p->data[0][y * p->linesize[0]];
            for(x = 0; x < s->width >> 1; x++){
                switch(s->bpp){
                case 32:
                    FFSWAP(uint32_t, ((uint32_t *)line)[x], ((uint32_t *)line)[s->width - x]);
                    break;
                case 24:
                    FFSWAP(uint8_t, ((uint8_t *)line)[3 * x    ], ((uint8_t *)line)[3 * s->width - 3 * x    ]);
                    FFSWAP(uint8_t, ((uint8_t *)line)[3 * x + 1], ((uint8_t *)line)[3 * s->width - 3 * x + 1]);
                    FFSWAP(uint8_t, ((uint8_t *)line)[3 * x + 2], ((uint8_t *)line)[3 * s->width - 3 * x + 2]);
                    break;
                case 16:
                    FFSWAP(uint16_t, ((uint16_t *)line)[x], ((uint16_t *)line)[s->width - x]);
                    break;
                case 8:
                    FFSWAP(uint8_t, ((uint8_t *)line)[x], ((uint8_t *)line)[s->width - x]);
                }
            }
        }
    }

    *picture= *(AVFrame*)&s->picture;
    *data_size = sizeof(AVPicture);

    return avpkt->size;
}
