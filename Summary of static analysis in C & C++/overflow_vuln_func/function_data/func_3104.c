static int init_image(TiffContext *s)
{
    int i, ret;
    uint32_t *pal;

    switch (s->bpp * 10 + s->bppcount) {
    case 11:
        if (!s->palette_is_set) {
            s->avctx->pix_fmt = AV_PIX_FMT_MONOBLACK;
            break;
        }
    case 21:
    case 41:
    case 81:
        s->avctx->pix_fmt = AV_PIX_FMT_PAL8;
        break;
    case 243:
        s->avctx->pix_fmt = AV_PIX_FMT_RGB24;
        break;
    case 161:
        s->avctx->pix_fmt = s->le ? AV_PIX_FMT_GRAY16LE : AV_PIX_FMT_GRAY16BE;
        break;
    case 162:
        s->avctx->pix_fmt = AV_PIX_FMT_GRAY8A;
        break;
    case 324:
        s->avctx->pix_fmt = AV_PIX_FMT_RGBA;
        break;
    case 483:
        s->avctx->pix_fmt = s->le ? AV_PIX_FMT_RGB48LE : AV_PIX_FMT_RGB48BE;
        break;
    case 644:
        s->avctx->pix_fmt = s->le ? AV_PIX_FMT_RGBA64LE : AV_PIX_FMT_RGBA64BE;
        break;
    default:
        av_log(s->avctx, AV_LOG_ERROR,
               "This format is not supported (bpp=%d, bppcount=%d)\n",
               s->bpp, s->bppcount);
        return AVERROR_INVALIDDATA;
    }
    if (s->width != s->avctx->width || s->height != s->avctx->height) {
        if ((ret = av_image_check_size(s->width, s->height, 0, s->avctx)) < 0)
            return ret;
        avcodec_set_dimensions(s->avctx, s->width, s->height);
    }
    if (s->picture.data[0])
        s->avctx->release_buffer(s->avctx, &s->picture);
    if ((ret = s->avctx->get_buffer(s->avctx, &s->picture)) < 0) {
        av_log(s->avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }
    if (s->avctx->pix_fmt == AV_PIX_FMT_PAL8) {
        if (s->palette_is_set) {
            memcpy(s->picture.data[1], s->palette, sizeof(s->palette));
        } else {
            /
            pal = (uint32_t *) s->picture.data[1];
            for (i = 0; i < 1<<s->bpp; i++)
                pal[i] = 0xFF << 24 | i * 255 / ((1<<s->bpp) - 1) * 0x010101;
        }
    }
    return 0;
}
