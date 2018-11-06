static void do_audio_out(AVFormatContext *s, 
                         AVOutputStream *ost, 
                         AVInputStream *ist,
                         unsigned char *buf, int size)
{
    uint8_t *buftmp;
    uint8_t audio_buf[2*MAX_AUDIO_PACKET_SIZE]; /
    uint8_t audio_out[4*MAX_AUDIO_PACKET_SIZE]; /
    int size_out, frame_bytes, ret;
    AVCodecContext *enc;

    enc = &ost->st->codec;

    if (ost->audio_resample) {
        buftmp = audio_buf;
        size_out = audio_resample(ost->resample, 
                                  (short *)buftmp, (short *)buf,
                                  size / (ist->st->codec.channels * 2));
        size_out = size_out * enc->channels * 2;
    } else {
        buftmp = buf;
        size_out = size;
    }

    /
    if (enc->frame_size > 1) {
        /
        fifo_write(&ost->fifo, buftmp, size_out, 
                   &ost->fifo.wptr);

        frame_bytes = enc->frame_size * 2 * enc->channels;
        
        while (fifo_read(&ost->fifo, audio_buf, frame_bytes, 
                     &ost->fifo.rptr) == 0) {
            ret = avcodec_encode_audio(enc, audio_out, sizeof(audio_out), 
                                       (short *)audio_buf);
            av_write_frame(s, ost->index, audio_out, ret);
        }
    } else {
        /
        /
        switch(enc->codec->id) {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            break;
        default:
            size_out = size_out >> 1;
            break;
        }
        ret = avcodec_encode_audio(enc, audio_out, size_out, 
				   (short *)buftmp);
        av_write_frame(s, ost->index, audio_out, ret);
    }
}
