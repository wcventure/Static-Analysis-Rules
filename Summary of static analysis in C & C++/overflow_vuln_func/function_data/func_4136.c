static int pcm_encode_frame(AVCodecContext *avctx,
			    unsigned char *frame, int buf_size, void *data)
{
    int n, sample_size, v;
    short *samples;
    unsigned char *dst;

    switch(avctx->codec->id) {
    case CODEC_ID_PCM_S16LE:
    case CODEC_ID_PCM_S16BE:
    case CODEC_ID_PCM_U16LE:
    case CODEC_ID_PCM_U16BE:
        sample_size = 2;
        break;
    default:
        sample_size = 1;
        break;
    }
    n = buf_size / sample_size;
    samples = data;
    dst = frame;

    switch(avctx->codec->id) {
    case CODEC_ID_PCM_S16LE:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = v & 0xff;
            dst[1] = v >> 8;
            dst += 2;
        }
        break;
    case CODEC_ID_PCM_S16BE:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = v >> 8;
            dst[1] = v;
            dst += 2;
        }
        break;
    case CODEC_ID_PCM_U16LE:
        for(;n>0;n--) {
            v = *samples++;
            v += 0x8000;
            dst[0] = v & 0xff;
            dst[1] = v >> 8;
            dst += 2;
        }
        break;
    case CODEC_ID_PCM_U16BE:
        for(;n>0;n--) {
            v = *samples++;
            v += 0x8000;
            dst[0] = v >> 8;
            dst[1] = v;
            dst += 2;
        }
        break;
    case CODEC_ID_PCM_S8:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = (v + 128) >> 8;
            dst++;
        }
        break;
    case CODEC_ID_PCM_U8:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = ((v + 128) >> 8) + 128;
            dst++;
        }
        break;
    case CODEC_ID_PCM_ALAW:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = linear_to_alaw[(v + 32768) >> 2];
            dst++;
        }
        break;
    case CODEC_ID_PCM_MULAW:
        for(;n>0;n--) {
            v = *samples++;
            dst[0] = linear_to_ulaw[(v + 32768) >> 2];
            dst++;
        }
        break;
    default:
        return -1;
    }
    avctx->key_frame = 1;
    //avctx->frame_size = (dst - frame) / (sample_size * avctx->channels);

    return dst - frame;
}
