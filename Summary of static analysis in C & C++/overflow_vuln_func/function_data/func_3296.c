static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame, AVPacket *avpkt)
{
    AVFrame *frame = data;
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    LclDecContext * const c = avctx->priv_data;
    unsigned int pixel_ptr;
    int row, col;
    unsigned char *encoded, *outptr;
    uint8_t *y_out, *u_out, *v_out;
    unsigned int width = avctx->width; // Real image width
    unsigned int height = avctx->height; // Real image height
    unsigned int mszh_dlen;
    unsigned char yq, y1q, uq, vq;
    int uqvq, ret;
    unsigned int mthread_inlen, mthread_outlen;
    unsigned int len = buf_size;

    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    outptr = frame->data[0]; // Output image pointer

    /
    switch (avctx->codec_id) {
    case AV_CODEC_ID_MSZH:
        switch (c->compression) {
        case COMP_MSZH:
            if (c->imgtype == IMGTYPE_RGB24 && len == width * height * 3) {
                ;
            } else if (c->flags & FLAG_MULTITHREAD) {
                mthread_inlen = AV_RL32(buf);
                if (len < 8) {
                    av_log(avctx, AV_LOG_ERROR, "len %d is too small\n", len);
                    return AVERROR_INVALIDDATA;
                }
                mthread_inlen = FFMIN(mthread_inlen, len - 8);
                mthread_outlen = AV_RL32(buf + 4);
                mthread_outlen = FFMIN(mthread_outlen, c->decomp_size);
                mszh_dlen = mszh_decomp(buf + 8, mthread_inlen, c->decomp_buf, c->decomp_size);
                if (mthread_outlen != mszh_dlen) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread1 decoded size differs (%d != %d)\n",
                           mthread_outlen, mszh_dlen);
                    return AVERROR_INVALIDDATA;
                }
                mszh_dlen = mszh_decomp(buf + 8 + mthread_inlen, len - 8 - mthread_inlen,
                                        c->decomp_buf + mthread_outlen, c->decomp_size - mthread_outlen);
                if (mthread_outlen != mszh_dlen) {
                    av_log(avctx, AV_LOG_ERROR, "Mthread2 decoded size differs (%d != %d)\n",
                           mthread_outlen, mszh_dlen);
                    return AVERROR_INVALIDDATA;
                }
                encoded = c->decomp_buf;
                len = c->decomp_size;
            } else {
                mszh_dlen = mszh_decomp(buf, len, c->decomp_buf, c->decomp_size);
                if (c->decomp_size != mszh_dlen) {
                    av_log(avctx, AV_LOG_ERROR, "Decoded size differs (%d != %d)\n",
                           c->decomp_size, mszh_dlen);
                    return AVERROR_INVALIDDATA;
                }
                encoded = c->decomp_buf;
                len = mszh_dlen;
            }
            break;
        case COMP_MSZH_NOCOMP: {
            int bppx2;
            switch (c->imgtype) {
            case IMGTYPE_YUV111:
            case IMGTYPE_RGB24:
                bppx2 = 6;
                break;
            case IMGTYPE_YUV422:
            case IMGTYPE_YUV211:
                bppx2 = 4;
                break;
            case IMGTYPE_YUV411:
            case IMGTYPE_YUV420:
                bppx2 = 3;
                break;
            default:
                bppx2 = 0; // will error out below
                break;
            }
            if (len < ((width * height * bppx2) >> 1))
                return AVERROR_INVALIDDATA;
            break;
        }
        default:
            av_log(avctx, AV_LOG_ERROR, "BUG! Unknown MSZH compression in frame decoder.\n");
            return AVERROR_INVALIDDATA;
        }
        break;
#if CONFIG_ZLIB_DECODER
    case AV_CODEC_ID_ZLIB:
        /
        if (c->compression == COMP_ZLIB_NORMAL && c->imgtype == IMGTYPE_RGB24 &&
            len == width * height * 3) {
            if (c->flags & FLAG_PNGFILTER) {
                memcpy(c->decomp_buf, buf, len);
                encoded = c->decomp_buf;
            } else {
                break;
            }
        } else if (c->flags & FLAG_MULTITHREAD) {
            mthread_inlen = AV_RL32(buf);
            mthread_inlen = FFMIN(mthread_inlen, len - 8);
            mthread_outlen = AV_RL32(buf + 4);
            mthread_outlen = FFMIN(mthread_outlen, c->decomp_size);
            ret = zlib_decomp(avctx, buf + 8, mthread_inlen, 0, mthread_outlen);
            if (ret < 0) return ret;
            ret = zlib_decomp(avctx, buf + 8 + mthread_inlen, len - 8 - mthread_inlen,
                              mthread_outlen, mthread_outlen);
            if (ret < 0) return ret;
        } else {
            int ret = zlib_decomp(avctx, buf, len, 0, c->decomp_size);
            if (ret < 0) return ret;
        }
        encoded = c->decomp_buf;
        len = c->decomp_size;
        break;
#endif
    default:
        av_log(avctx, AV_LOG_ERROR, "BUG! Unknown codec in frame decoder compression switch.\n");
        return AVERROR_INVALIDDATA;
    }


    /
    if (avctx->codec_id == AV_CODEC_ID_ZLIB && (c->flags & FLAG_PNGFILTER)) {
        switch (c->imgtype) {
        case IMGTYPE_YUV111:
        case IMGTYPE_RGB24:
            for (row = 0; row < height; row++) {
                pixel_ptr = row * width * 3;
                yq = encoded[pixel_ptr++];
                uqvq = AV_RL16(encoded+pixel_ptr);
                pixel_ptr += 2;
                for (col = 1; col < width; col++) {
                    encoded[pixel_ptr] = yq -= encoded[pixel_ptr];
                    uqvq -= AV_RL16(encoded+pixel_ptr+1);
                    AV_WL16(encoded+pixel_ptr+1, uqvq);
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
            return AVERROR_INVALIDDATA;
        }
    }

    /
    y_out = frame->data[0] + (height - 1) * frame->linesize[0];
    u_out = frame->data[1] + (height - 1) * frame->linesize[1];
    v_out = frame->data[2] + (height - 1) * frame->linesize[2];
    switch (c->imgtype) {
    case IMGTYPE_YUV111:
        for (row = 0; row < height; row++) {
            for (col = 0; col < width; col++) {
                y_out[col] = *encoded++;
                u_out[col] = *encoded++ + 128;
                v_out[col] = *encoded++ + 128;
            }
            y_out -= frame->linesize[0];
            u_out -= frame->linesize[1];
            v_out -= frame->linesize[2];
        }
        break;
    case IMGTYPE_YUV422:
        for (row = 0; row < height; row++) {
            for (col = 0; col < width - 3; col += 4) {
                memcpy(y_out + col, encoded, 4);
                encoded += 4;
                u_out[ col >> 1     ] = *encoded++ + 128;
                u_out[(col >> 1) + 1] = *encoded++ + 128;
                v_out[ col >> 1     ] = *encoded++ + 128;
                v_out[(col >> 1) + 1] = *encoded++ + 128;
            }
            y_out -= frame->linesize[0];
            u_out -= frame->linesize[1];
            v_out -= frame->linesize[2];
        }
        break;
    case IMGTYPE_RGB24:
        for (row = height - 1; row >= 0; row--) {
            pixel_ptr = row * frame->linesize[0];
            memcpy(outptr + pixel_ptr, encoded, 3 * width);
            encoded += 3 * width;
        }
        break;
    case IMGTYPE_YUV411:
        for (row = 0; row < height; row++) {
            for (col = 0; col < width - 3; col += 4) {
                memcpy(y_out + col, encoded, 4);
                encoded += 4;
                u_out[col >> 2] = *encoded++ + 128;
                v_out[col >> 2] = *encoded++ + 128;
            }
            y_out -= frame->linesize[0];
            u_out -= frame->linesize[1];
            v_out -= frame->linesize[2];
        }
        break;
    case IMGTYPE_YUV211:
        for (row = 0; row < height; row++) {
            for (col = 0; col < width - 1; col += 2) {
                memcpy(y_out + col, encoded, 2);
                encoded += 2;
                u_out[col >> 1] = *encoded++ + 128;
                v_out[col >> 1] = *encoded++ + 128;
            }
            y_out -= frame->linesize[0];
            u_out -= frame->linesize[1];
            v_out -= frame->linesize[2];
        }
        break;
    case IMGTYPE_YUV420:
        u_out = frame->data[1] + ((height >> 1) - 1) * frame->linesize[1];
        v_out = frame->data[2] + ((height >> 1) - 1) * frame->linesize[2];
        for (row = 0; row < height - 1; row += 2) {
            for (col = 0; col < width - 1; col += 2) {
                memcpy(y_out + col, encoded, 2);
                encoded += 2;
                memcpy(y_out + col - frame->linesize[0], encoded, 2);
                encoded += 2;
                u_out[col >> 1] = *encoded++ + 128;
                v_out[col >> 1] = *encoded++ + 128;
            }
            y_out -= frame->linesize[0] << 1;
            u_out -= frame->linesize[1];
            v_out -= frame->linesize[2];
        }
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "BUG! Unknown imagetype in image decoder.\n");
        return AVERROR_INVALIDDATA;
    }

    *got_frame = 1;

    /
    return buf_size;
}
