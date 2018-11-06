static int decode_frame(AVCodecContext *avctx, void *data, int *data_size, uint8_t *buf, int buf_size)
{
	LclContext * const c = (LclContext *)avctx->priv_data;
	unsigned char *encoded = (unsigned char *)buf;
    int pixel_ptr;
    int row, col;
    unsigned char *outptr;
    unsigned int width = avctx->width; // Real image width
    unsigned int height = avctx->height; // Real image height
    unsigned int mszh_dlen;
    unsigned char yq, y1q, uq, vq;
    int uqvq;
    unsigned int mthread_inlen, mthread_outlen;
#ifdef CONFIG_ZLIB
    int zret; // Zlib return code
#endif
    int len = buf_size;

	/
	if (buf_size == 0)
		return 0;

	if(c->pic.data[0])
		avctx->release_buffer(avctx, &c->pic);

	c->pic.reference = 0;
	c->pic.buffer_hints = FF_BUFFER_HINTS_VALID;
	if(avctx->get_buffer(avctx, &c->pic) < 0){
		av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
		return -1;
	}

    outptr = c->pic.data[0]; // Output image pointer

    /
    switch (avctx->codec_id) {
        case CODEC_ID_MSZH:
            switch (c->compression) {
                case COMP_MSZH:
                    if (c->flags & FLAG_MULTITHREAD) {
                        mthread_inlen = *((unsigned int*)encoded);
                        mthread_outlen = *((unsigned int*)(encoded+4));
                        mszh_dlen = mszh_decomp(encoded + 8, mthread_inlen, c->decomp_buf);
                        if (mthread_outlen != mszh_dlen) {
                            av_log(avctx, AV_LOG_ERROR, "Mthread1 decoded size differs (%d != %d)\n",
                                   mthread_outlen, mszh_dlen);
                        }
                        mszh_dlen = mszh_decomp(encoded + 8 + mthread_inlen, len - mthread_inlen,
                                                c->decomp_buf + mthread_outlen);
                        if ((c->decomp_size - mthread_outlen) != mszh_dlen) {
                            av_log(avctx, AV_LOG_ERROR, "Mthread2 decoded size differs (%d != %d)\n",
                                   c->decomp_size - mthread_outlen, mszh_dlen);
                        }
                        encoded = c->decomp_buf;
                        len = c->decomp_size;
                    } else {
                        mszh_dlen = mszh_decomp(encoded, len, c->decomp_buf);
                        if (c->decomp_size != mszh_dlen) {
                            av_log(avctx, AV_LOG_ERROR, "Decoded size differs (%d != %d)\n",
                                   c->decomp_size, mszh_dlen);
                        }
                        encoded = c->decomp_buf;
                        len = mszh_dlen;
                    }
                    break;
                case COMP_MSZH_NOCOMP:
                    break;
                default:
                    av_log(avctx, AV_LOG_ERROR, "BUG! Unknown MSZH compression in frame decoder.\n");
                    return -1;
            }
            break;
        case CODEC_ID_ZLIB:
#ifdef CONFIG_ZLIB
            /
            if ((c->compression == COMP_ZLIB_NORMAL) && (c->imgtype == IMGTYPE_RGB24) &&
               (len == width * height * 3))
                break;
            zret = inflateReset(&(c->zstream));
            if (zret != Z_OK) {
                av_log(avctx, AV_LOG_ERROR, "Inflate reset error: %d\n", zret);
                return -1;
            }
            if (c->flags & FLAG_MULTITHREAD) {
                mthread_inlen = *((unsigned int*)encoded);
                mthread_outlen = *((unsigned int*)(encoded+4));
                c->zstream.next_in = encoded + 8;
                c->zstream.avail_in = mthread_inlen;
                c->zstream.next_out = c->decomp_buf;
                c->zstream.avail_out = mthread_outlen;    
                zret = inflate(&(c->zstream), Z_FINISH);
                if ((zret != Z_OK) && (zret != Z_STREAM_END)) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread1 inflate error: %d\n", zret);
                    return -1;
                }
                if (mthread_outlen != (unsigned int)(c->zstream.total_out)) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread1 decoded size differs (%u != %lu)\n",
                           mthread_outlen, c->zstream.total_out);
                }
                zret = inflateReset(&(c->zstream));
                if (zret != Z_OK) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread2 inflate reset error: %d\n", zret);
                    return -1;
                }
                c->zstream.next_in = encoded + 8 + mthread_inlen;
                c->zstream.avail_in = len - mthread_inlen;
                c->zstream.next_out = c->decomp_buf + mthread_outlen;
                c->zstream.avail_out = mthread_outlen;    
                zret = inflate(&(c->zstream), Z_FINISH);
                if ((zret != Z_OK) && (zret != Z_STREAM_END)) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread2 inflate error: %d\n", zret);
                    return -1;
                }
                if ((c->decomp_size - mthread_outlen) != (unsigned int)(c->zstream.total_out)) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread2 decoded size differs (%d != %lu)\n",
                           c->decomp_size - mthread_outlen, c->zstream.total_out);
                }
            } else {
                c->zstream.next_in = encoded;
                c->zstream.avail_in = len;
                c->zstream.next_out = c->decomp_buf;
                c->zstream.avail_out = c->decomp_size;
                zret = inflate(&(c->zstream), Z_FINISH);
                if ((zret != Z_OK) && (zret != Z_STREAM_END)) {
                    av_log(avctx, AV_LOG_ERROR, "Inflate error: %d\n", zret);
                    return -1;
                }
                if (c->decomp_size != (unsigned int)(c->zstream.total_out)) {
                    av_log(avctx, AV_LOG_ERROR, "Decoded size differs (%d != %lu)\n",
                           c->decomp_size, c->zstream.total_out);
                }
            }
            encoded = c->decomp_buf;
            len = c->decomp_size;;
#else
            av_log(avctx, AV_LOG_ERROR, "BUG! Zlib support not compiled in frame decoder.\n");
            return -1;
#endif
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "BUG! Unknown codec in frame decoder compression switch.\n");
            return -1;
    }


    /
    if ((avctx->codec_id == CODEC_ID_ZLIB) && (c->flags & FLAG_PNGFILTER)) {
        switch (c->imgtype) {
            case IMGTYPE_YUV111:
            case IMGTYPE_RGB24:
                for (row = 0; row < height; row++) {
                    pixel_ptr = row * width * 3;
                    yq = encoded[pixel_ptr++];
                    uqvq = encoded[pixel_ptr++];
              	  uqvq+=(encoded[pixel_ptr++] << 8);
                    for (col = 1; col < width; col++) {
                        encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                        uqvq -= (encoded[pixel_ptr+1] | (encoded[pixel_ptr+2]<<8));
                        encoded[pixel_ptr+1] = (uqvq) & 0xff;
                        encoded[pixel_ptr+2] = ((uqvq)>>8) & 0xff;
                        pixel_ptr += 3;
                    }
                }
                break;
            case IMGTYPE_YUV422:
                for (row = 0; row < height; row++) {
                    pixel_ptr = row * width * 2;
                    yq = uq = vq =0;
                    for (col = 0; col < width/4; col++) {
                        encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                        encoded[pixel_ptr+1] = yq -= encoded[pixel_ptr+1];
                        encoded[pixel_ptr+2] = yq -= encoded[pixel_ptr+2];
                        encoded[pixel_ptr+3] = yq -= encoded[pixel_ptr+3];
                        encoded[pixel_ptr+4] = uq -= encoded[pixel_ptr+4];
                        encoded[pixel_ptr+5] = uq -= encoded[pixel_ptr+5];
                        encoded[pixel_ptr+6] = vq -= encoded[pixel_ptr+6];
                        encoded[pixel_ptr+7] = vq -= encoded[pixel_ptr+7];
                        pixel_ptr += 8;
                    }
                }
                break;
            case IMGTYPE_YUV411:
                for (row = 0; row < height; row++) {
                    pixel_ptr = row * width / 2 * 3;
                    yq = uq = vq =0;
                    for (col = 0; col < width/4; col++) {
                        encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                        encoded[pixel_ptr+1] = yq -= encoded[pixel_ptr+1];
                        encoded[pixel_ptr+2] = yq -= encoded[pixel_ptr+2];
                        encoded[pixel_ptr+3] = yq -= encoded[pixel_ptr+3];
                        encoded[pixel_ptr+4] = uq -= encoded[pixel_ptr+4];
                        encoded[pixel_ptr+5] = vq -= encoded[pixel_ptr+5];
                        pixel_ptr += 6;
                    }
                }
                break;
            case IMGTYPE_YUV211:
                for (row = 0; row < height; row++) {
                    pixel_ptr = row * width * 2;
                    yq = uq = vq =0;
                    for (col = 0; col < width/2; col++) {
                        encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                        encoded[pixel_ptr+1] = yq -= encoded[pixel_ptr+1];
                        encoded[pixel_ptr+2] = uq -= encoded[pixel_ptr+2];
                        encoded[pixel_ptr+3] = vq -= encoded[pixel_ptr+3];
                        pixel_ptr += 4;
                    }
                }
                break;
            case IMGTYPE_YUV420:
                for (row = 0; row < height/2; row++) {
                    pixel_ptr = row * width * 3;
                    yq = y1q = uq = vq =0;
                    for (col = 0; col < width/2; col++) {
                        encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                        encoded[pixel_ptr+1] = yq -= encoded[pixel_ptr+1];
                        encoded[pixel_ptr+2] = y1q -= encoded[pixel_ptr+2];
                        encoded[pixel_ptr+3] = y1q -= encoded[pixel_ptr+3];
                        encoded[pixel_ptr+4] = uq -= encoded[pixel_ptr+4];
                        encoded[pixel_ptr+5] = vq -= encoded[pixel_ptr+5];
                        pixel_ptr += 6;
                    }
                }
                break;
            default:
                av_log(avctx, AV_LOG_ERROR, "BUG! Unknown imagetype in pngfilter switch.\n");
                return -1;
        }
    }

    /
    switch (c->imgtype) {
        case IMGTYPE_YUV111:
            for (row = height - 1; row >= 0; row--) {
                pixel_ptr = row * c->pic.linesize[0];
                for (col = 0; col < width; col++) {
                    outptr[pixel_ptr++] = get_b(encoded[0], encoded[1]);
                    outptr[pixel_ptr++] = get_g(encoded[0], encoded[1], encoded[2]);
                    outptr[pixel_ptr++] = get_r(encoded[0], encoded[2]);
                    encoded += 3;
                }
            }
            break;
        case IMGTYPE_YUV422:
            for (row = height - 1; row >= 0; row--) {
                pixel_ptr = row * c->pic.linesize[0];
                for (col = 0; col < width/4; col++) {
                    outptr[pixel_ptr++] = get_b(encoded[0], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[0], encoded[4], encoded[6]);
                    outptr[pixel_ptr++] = get_r(encoded[0], encoded[6]);
                    outptr[pixel_ptr++] = get_b(encoded[1], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[1], encoded[4], encoded[6]);
                    outptr[pixel_ptr++] = get_r(encoded[1], encoded[6]);
                    outptr[pixel_ptr++] = get_b(encoded[2], encoded[5]);
                    outptr[pixel_ptr++] = get_g(encoded[2], encoded[5], encoded[7]);
                    outptr[pixel_ptr++] = get_r(encoded[2], encoded[7]);
                    outptr[pixel_ptr++] = get_b(encoded[3], encoded[5]);
                    outptr[pixel_ptr++] = get_g(encoded[3], encoded[5], encoded[7]);
                    outptr[pixel_ptr++] = get_r(encoded[3], encoded[7]);
                    encoded += 8;
                }
            }
            break;
        case IMGTYPE_RGB24:
            for (row = height - 1; row >= 0; row--) {
                pixel_ptr = row * c->pic.linesize[0];
                for (col = 0; col < width; col++) {
                    outptr[pixel_ptr++] = encoded[0];
                    outptr[pixel_ptr++] = encoded[1];
                    outptr[pixel_ptr++] = encoded[2];
                    encoded += 3;
                }
            }
            break;
        case IMGTYPE_YUV411:
            for (row = height - 1; row >= 0; row--) {
                pixel_ptr = row * c->pic.linesize[0];
                for (col = 0; col < width/4; col++) {
                    outptr[pixel_ptr++] = get_b(encoded[0], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[0], encoded[4], encoded[5]);
                    outptr[pixel_ptr++] = get_r(encoded[0], encoded[5]);
                    outptr[pixel_ptr++] = get_b(encoded[1], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[1], encoded[4], encoded[5]);
                    outptr[pixel_ptr++] = get_r(encoded[1], encoded[5]);
                    outptr[pixel_ptr++] = get_b(encoded[2], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[2], encoded[4], encoded[5]);
                    outptr[pixel_ptr++] = get_r(encoded[2], encoded[5]);
                    outptr[pixel_ptr++] = get_b(encoded[3], encoded[4]);
                    outptr[pixel_ptr++] = get_g(encoded[3], encoded[4], encoded[5]);
                    outptr[pixel_ptr++] = get_r(encoded[3], encoded[5]);
                    encoded += 6;
                }
            }
            break;
        case IMGTYPE_YUV211:
            for (row = height - 1; row >= 0; row--) {
                pixel_ptr = row * c->pic.linesize[0];
                for (col = 0; col < width/2; col++) {
                    outptr[pixel_ptr++] = get_b(encoded[0], encoded[2]);
                    outptr[pixel_ptr++] = get_g(encoded[0], encoded[2], encoded[3]);
                    outptr[pixel_ptr++] = get_r(encoded[0], encoded[3]);
                    outptr[pixel_ptr++] = get_b(encoded[1], encoded[2]);
                    outptr[pixel_ptr++] = get_g(encoded[1], encoded[2], encoded[3]);
                    outptr[pixel_ptr++] = get_r(encoded[1], encoded[3]);
                    encoded += 4;
                }
            }
            break;
        case IMGTYPE_YUV420:
            for (row = height / 2 - 1; row >= 0; row--) {
                pixel_ptr = 2 * row * c->pic.linesize[0];
                for (col = 0; col < width/2; col++) {
                    outptr[pixel_ptr] = get_b(encoded[0], encoded[4]);
                    outptr[pixel_ptr+1] = get_g(encoded[0], encoded[4], encoded[5]);
                    outptr[pixel_ptr+2] = get_r(encoded[0], encoded[5]);
                    outptr[pixel_ptr+3] = get_b(encoded[1], encoded[4]);
                    outptr[pixel_ptr+4] = get_g(encoded[1], encoded[4], encoded[5]);
                    outptr[pixel_ptr+5] = get_r(encoded[1], encoded[5]);
                    outptr[pixel_ptr-c->pic.linesize[0]] = get_b(encoded[2], encoded[4]);
                    outptr[pixel_ptr-c->pic.linesize[0]+1] = get_g(encoded[2], encoded[4], encoded[5]);
                    outptr[pixel_ptr-c->pic.linesize[0]+2] = get_r(encoded[2], encoded[5]);
                    outptr[pixel_ptr-c->pic.linesize[0]+3] = get_b(encoded[3], encoded[4]);
                    outptr[pixel_ptr-c->pic.linesize[0]+4] = get_g(encoded[3], encoded[4], encoded[5]);
                    outptr[pixel_ptr-c->pic.linesize[0]+5] = get_r(encoded[3], encoded[5]);
                    pixel_ptr += 6;
                    encoded += 6;
                }
            }
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "BUG! Unknown imagetype in image decoder.\n");
            return -1;
    }

    *data_size = sizeof(AVFrame);
    *(AVFrame*)data = c->pic;

    /
    return buf_size;
}
