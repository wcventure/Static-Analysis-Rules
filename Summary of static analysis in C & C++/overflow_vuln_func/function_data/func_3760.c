static int avisynth_read_packet_audio(AVFormatContext *s, AVPacket *pkt, int discard) {
    AviSynthContext *avs = s->priv_data;
    AVRational fps, samplerate;
    int samples;
    const char* error;

    if (avs->curr_sample >= avs->vi->num_audio_samples)
        return AVERROR_EOF;

    fps.num = avs->vi->fps_numerator;
    fps.den = avs->vi->fps_denominator;
    samplerate.num = avs->vi->audio_samples_per_second;
    samplerate.den = 1;

    if (avs_has_video(avs->vi)) {
        if (avs->curr_frame < avs->vi->num_frames)
            samples = av_rescale_q(avs->curr_frame, samplerate, fps) - avs->curr_sample;
        else
            samples = av_rescale_q(1, samplerate, fps);
    } else {
        samples = 1000;
    }

    // After seeking, audio may catch up with video.
    if (samples <= 0) {
        pkt->size = 0;
        pkt->data = NULL;
        return 0;
    }

    if (avs->curr_sample + samples > avs->vi->num_audio_samples)
        samples = avs->vi->num_audio_samples - avs->curr_sample;

    // This must happen even if the stream is discarded to prevent desync.
    avs->curr_sample += samples;
    if (discard)
        return 0;

    pkt->pts = avs->curr_sample;
    pkt->dts = avs->curr_sample;
    pkt->duration = samples;

    pkt->size = avs_bytes_per_channel_sample(avs->vi) * samples * avs->vi->nchannels;
    if (!pkt->size)
        return AVERROR_UNKNOWN;
    pkt->data = av_malloc(pkt->size);
    if (!pkt->data)
        return AVERROR_UNKNOWN;

    avs_library->avs_get_audio(avs->clip, pkt->data, avs->curr_sample, samples);
    error = avs_library->avs_clip_get_error(avs->clip);
    if (error) {
        av_log(s, AV_LOG_ERROR, "%s\n", error);
        avs->error = 1;
        av_freep(&pkt->data);
        return AVERROR_UNKNOWN;
    }
    return 0;
}
