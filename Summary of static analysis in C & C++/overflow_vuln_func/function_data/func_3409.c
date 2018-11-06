static int alloc_audio_output_buf(AVCodecContext *dec, AVCodecContext *enc,
                                  int nb_samples, int *buf_linesize)
{
    int64_t audio_buf_samples;
    int audio_buf_size;

    /
    audio_buf_samples = ((int64_t)nb_samples * enc->sample_rate + dec->sample_rate) /
                        dec->sample_rate;
    audio_buf_samples = 4 * audio_buf_samples + 10000; // safety factors for resampling
    audio_buf_samples = FFMAX(audio_buf_samples, enc->frame_size);
    if (audio_buf_samples > INT_MAX)
        return AVERROR(EINVAL);

    audio_buf_size = av_samples_get_buffer_size(buf_linesize, enc->channels,
                                                audio_buf_samples,
                                                enc->sample_fmt, 0);
    if (audio_buf_size < 0)
        return audio_buf_size;

    av_fast_malloc(&audio_buf, &allocated_audio_buf_size, audio_buf_size);
    if (!audio_buf)
        return AVERROR(ENOMEM);

    return 0;
}
