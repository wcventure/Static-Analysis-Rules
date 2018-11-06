static int decode_init(AVCodecContext *avctx)
{
    LclContext * const c = (LclContext *)avctx->priv_data;
    int basesize = avctx->width * avctx->height;
    int zret; // Zlib return code

    c->avctx = avctx;
    avctx->has_b_frames = 0;

    c->pic.data[0] = NULL;

#ifdef CONFIG_ZLIB
    // Needed if zlib unused or init aborted before inflateInit
    memset(&(c->zstream), 0, sizeof(z_stream)); 
#endif

    if (avctx->extradata_size < 8) {
        av_log(avctx, AV_LOG_ERROR, "Extradata size too small.\n");
        return 1;
    }

    / 
    if (((avctx->codec_id == CODEC_ID_MSZH)  && (*((char *)avctx->extradata + 7) != CODEC_MSZH)) ||
        ((avctx->codec_id == CODEC_ID_ZLIB)  && (*((char *)avctx->extradata + 7) != CODEC_ZLIB))) {
        av_log(avctx, AV_LOG_ERROR, "Codec id and codec type mismatch. This should not happen.\n");
    }

    /
    switch (c->imgtype = *((char *)avctx->extradata + 4)) {
        case IMGTYPE_YUV111:
            c->decomp_size = basesize * 3;
            av_log(avctx, AV_LOG_INFO, "Image type is YUV 1:1:1.\n");
            break;
        case IMGTYPE_YUV422:
            c->decomp_size = basesize * 2;
            av_log(avctx, AV_LOG_INFO, "Image type is YUV 4:2:2.\n");
            break;
        case IMGTYPE_RGB24:
            c->decomp_size = basesize * 3;
            av_log(avctx, AV_LOG_INFO, "Image type is RGB 24.\n");
            break;
        case IMGTYPE_YUV411:
            c->decomp_size = basesize / 2 * 3;
            av_log(avctx, AV_LOG_INFO, "Image type is YUV 4:1:1.\n");
            break;
        case IMGTYPE_YUV211:
            c->decomp_size = basesize * 2;
            av_log(avctx, AV_LOG_INFO, "Image type is YUV 2:1:1.\n");
            break;
        case IMGTYPE_YUV420:
            c->decomp_size = basesize / 2 * 3;
            av_log(avctx, AV_LOG_INFO, "Image type is YUV 4:2:0.\n");
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "Unsupported image format %d.\n", c->imgtype);
            return 1;
    }

    /
    c->compression = *((char *)avctx->extradata + 5);
    switch (avctx->codec_id) {
        case CODEC_ID_MSZH:
            switch (c->compression) {
                case COMP_MSZH:
                    av_log(avctx, AV_LOG_INFO, "Compression enabled.\n");
                    break;
                case COMP_MSZH_NOCOMP:
                    c->decomp_size = 0;
                    av_log(avctx, AV_LOG_INFO, "No compression.\n");
                    break;
                default:
                    av_log(avctx, AV_LOG_ERROR, "Unsupported compression format for MSZH (%d).\n", c->compression);
                    return 1;
            }
            break;
        case CODEC_ID_ZLIB:
#ifdef CONFIG_ZLIB
            switch (c->compression) {
                case COMP_ZLIB_HISPEED:
                    av_log(avctx, AV_LOG_INFO, "High speed compression.\n");
                    break;
                case COMP_ZLIB_HICOMP:
                    av_log(avctx, AV_LOG_INFO, "High compression.\n");
                    break;
                case COMP_ZLIB_NORMAL:
                    av_log(avctx, AV_LOG_INFO, "Normal compression.\n");
                    break;
                default:
                    if ((c->compression < Z_NO_COMPRESSION) || (c->compression > Z_BEST_COMPRESSION)) {
                	    av_log(avctx, AV_LOG_ERROR, "Unusupported compression level for ZLIB: (%d).\n", c->compression);
                        return 1;
                    }
                    av_log(avctx, AV_LOG_INFO, "Compression level for ZLIB: (%d).\n", c->compression);
            }
#else
            av_log(avctx, AV_LOG_ERROR, "Zlib support not compiled.\n");
            return 1;
#endif
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "BUG! Unknown codec in compression switch.\n");
            return 1;
    }

    /
    /
    if (c->decomp_size) {
        if ((c->decomp_buf = av_malloc(c->decomp_size+4*8)) == NULL) {
            av_log(avctx, AV_LOG_ERROR, "Can't allocate decompression buffer.\n");
            return 1;
        }
    }
  
    / 
    c->flags = *((char *)avctx->extradata + 6);
    if (c->flags & FLAG_MULTITHREAD)
        av_log(avctx, AV_LOG_INFO, "Multithread encoder flag set.\n");
    if (c->flags & FLAG_NULLFRAME)
        av_log(avctx, AV_LOG_INFO, "Nullframe insertion flag set.\n");
    if ((avctx->codec_id == CODEC_ID_ZLIB) && (c->flags & FLAG_PNGFILTER))
        av_log(avctx, AV_LOG_INFO, "PNG filter flag set.\n");
    if (c->flags & FLAGMASK_UNUSED)
        av_log(avctx, AV_LOG_ERROR, "Unknown flag set (%d).\n", c->flags);

    /
    if (avctx->codec_id == CODEC_ID_ZLIB) {
#ifdef CONFIG_ZLIB
        c->zstream.zalloc = Z_NULL;
        c->zstream.zfree = Z_NULL;
        c->zstream.opaque = Z_NULL;
        zret = inflateInit(&(c->zstream));
        if (zret != Z_OK) {
            av_log(avctx, AV_LOG_ERROR, "Inflate init error: %d\n", zret);
            return 1;
        }
#else
    av_log(avctx, AV_LOG_ERROR, "Zlib support not compiled.\n");
    return 1;
#endif
    }

    avctx->pix_fmt = PIX_FMT_BGR24;

    return 0;
}
