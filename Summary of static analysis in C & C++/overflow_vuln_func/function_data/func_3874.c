static int pcm_decode_frame(AVCodecContext *avctx,
                            void *data, int *data_size,
                            AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    PCMDecode *s = avctx->priv_data;
    int sample_size, c, n, i;
    short *samples;
    const uint8_t *src, *src8, *src2[MAX_CHANNELS];
    uint8_t *dstu8;
    int16_t *dst_int16_t;
    int32_t *dst_int32_t;
    int64_t *dst_int64_t;
    uint16_t *dst_uint16_t;
    uint32_t *dst_uint32_t;

    samples = data;
    src = buf;

    if (avctx->sample_fmt!=avctx->codec->sample_fmts[0]) {
        av_log(avctx, AV_LOG_ERROR, "invalid sample_fmt\n");
        return -1;
    }

    if(avctx->channels <= 0 || avctx->channels > MAX_CHANNELS){
        av_log(avctx, AV_LOG_ERROR, "PCM channels out of bounds\n");
        return -1;
    }

    sample_size = av_get_bits_per_sample(avctx->codec_id)/8;

    /
    if (CODEC_ID_PCM_DVD == avctx->codec_id)
        /
        sample_size = avctx->bits_per_coded_sample * 2 / 8;
    else if (avctx->codec_id == CODEC_ID_PCM_LXF)
        /
        sample_size = 5;

    if (sample_size == 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid sample_size\n");
        return AVERROR(EINVAL);
    }

    n = avctx->channels * sample_size;

    if(n && buf_size % n){
        if (buf_size < n) {
            av_log(avctx, AV_LOG_ERROR, "invalid PCM packet\n");
            return -1;
        }else
            buf_size -= buf_size % n;
    }

    buf_size= FFMIN(buf_size, *data_size/2);
    *data_size=0;

    n = buf_size/sample_size;

    switch(avctx->codec->id) {
    case CODEC_ID_PCM_U32LE:
        DECODE(uint32_t, le32, src, samples, n, 0, 0x80000000)
        break;
    case CODEC_ID_PCM_U32BE:
        DECODE(uint32_t, be32, src, samples, n, 0, 0x80000000)
        break;
    case CODEC_ID_PCM_S24LE:
        DECODE(int32_t, le24, src, samples, n, 8, 0)
        break;
    case CODEC_ID_PCM_S24BE:
        DECODE(int32_t, be24, src, samples, n, 8, 0)
        break;
    case CODEC_ID_PCM_U24LE:
        DECODE(uint32_t, le24, src, samples, n, 8, 0x800000)
        break;
    case CODEC_ID_PCM_U24BE:
        DECODE(uint32_t, be24, src, samples, n, 8, 0x800000)
        break;
    case CODEC_ID_PCM_S24DAUD:
        for(;n>0;n--) {
          uint32_t v = bytestream_get_be24(&src);
          v >>= 4; // sync flags are here
          *samples++ = av_reverse[(v >> 8) & 0xff] +
                       (av_reverse[v & 0xff] << 8);
        }
        break;
    case CODEC_ID_PCM_S16LE_PLANAR:
        n /= avctx->channels;
        for(c=0;c<avctx->channels;c++)
            src2[c] = &src[c*n*2];
        for(;n>0;n--)
            for(c=0;c<avctx->channels;c++)
                *samples++ = bytestream_get_le16(&src2[c]);
        src = src2[avctx->channels-1];
        break;
    case CODEC_ID_PCM_U16LE:
        DECODE(uint16_t, le16, src, samples, n, 0, 0x8000)
        break;
    case CODEC_ID_PCM_U16BE:
        DECODE(uint16_t, be16, src, samples, n, 0, 0x8000)
        break;
    case CODEC_ID_PCM_S8:
        dstu8= (uint8_t*)samples;
        for(;n>0;n--) {
            *dstu8++ = *src++ + 128;
        }
        samples= (short*)dstu8;
        break;
#if HAVE_BIGENDIAN
    case CODEC_ID_PCM_F64LE:
        DECODE(int64_t, le64, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_S32LE:
    case CODEC_ID_PCM_F32LE:
        DECODE(int32_t, le32, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_S16LE:
        DECODE(int16_t, le16, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_F64BE:
    case CODEC_ID_PCM_F32BE:
    case CODEC_ID_PCM_S32BE:
    case CODEC_ID_PCM_S16BE:
#else
    case CODEC_ID_PCM_F64BE:
        DECODE(int64_t, be64, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_F32BE:
    case CODEC_ID_PCM_S32BE:
        DECODE(int32_t, be32, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_S16BE:
        DECODE(int16_t, be16, src, samples, n, 0, 0)
        break;
    case CODEC_ID_PCM_F64LE:
    case CODEC_ID_PCM_F32LE:
    case CODEC_ID_PCM_S32LE:
    case CODEC_ID_PCM_S16LE:
#endif /
    case CODEC_ID_PCM_U8:
        memcpy(samples, src, n*sample_size);
        src += n*sample_size;
        samples = (short*)((uint8_t*)data + n*sample_size);
        break;
    case CODEC_ID_PCM_ZORK:
        for(;n>0;n--) {
            int x= *src++;
            if(x&128) x-= 128;
            else      x = -x;
            *samples++ = x << 8;
        }
        break;
    case CODEC_ID_PCM_ALAW:
    case CODEC_ID_PCM_MULAW:
        for(;n>0;n--) {
            *samples++ = s->table[*src++];
        }
        break;
    case CODEC_ID_PCM_DVD:
        dst_int32_t = data;
        n /= avctx->channels;
        switch (avctx->bits_per_coded_sample) {
        case 20:
            while (n--) {
                c = avctx->channels;
                src8 = src + 4*c;
                while (c--) {
                    *dst_int32_t++ = (bytestream_get_be16(&src) << 16) + ((*src8   &0xf0) << 8);
                    *dst_int32_t++ = (bytestream_get_be16(&src) << 16) + ((*src8++ &0x0f) << 12);
                }
                src = src8;
            }
            break;
        case 24:
            while (n--) {
                c = avctx->channels;
                src8 = src + 4*c;
                while (c--) {
                    *dst_int32_t++ = (bytestream_get_be16(&src) << 16) + ((*src8++) << 8);
                    *dst_int32_t++ = (bytestream_get_be16(&src) << 16) + ((*src8++) << 8);
                }
                src = src8;
            }
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "PCM DVD unsupported sample depth\n");
            return -1;
        }
        samples = (short *) dst_int32_t;
        break;
    case CODEC_ID_PCM_LXF:
        dst_int32_t = data;
        n /= avctx->channels;
        //unpack and de-planerize
        for (i = 0; i < n; i++) {
            for (c = 0, src8 = src + i*5; c < avctx->channels; c++, src8 += n*5) {
                //extract low 20 bits and expand to 32 bits
                *dst_int32_t++ = (src8[2] << 28) | (src8[1] << 20) | (src8[0] << 12) |
                                 ((src8[2] & 0xF) << 8) | src8[1];
            }

            for (c = 0, src8 = src + i*5; c < avctx->channels; c++, src8 += n*5) {
                //extract high 20 bits and expand to 32 bits
                *dst_int32_t++ = (src8[4] << 24) | (src8[3] << 16) |
                                 ((src8[2] & 0xF0) << 8) | (src8[4] << 4) | (src8[3] >> 4);
            }
        }
        src += n * avctx->channels * 5;
        samples = (short *) dst_int32_t;
        break;
    default:
        return -1;
    }
    *data_size = (uint8_t *)samples - (uint8_t *)data;
    return src - buf;
}
