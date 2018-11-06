static int mxf_compute_sample_count(MXFContext *mxf, int stream_index, uint64_t *sample_count)
{
    int i, total = 0, size = 0;
    AVStream *st = mxf->fc->streams[stream_index];
    MXFTrack *track = st->priv_data;
    AVRational time_base = av_inv_q(track->edit_rate);
    AVRational sample_rate = av_inv_q(st->time_base);
    const MXFSamplesPerFrame *spf = NULL;

    if ((sample_rate.num / sample_rate.den) == 48000)
        spf = ff_mxf_get_samples_per_frame(mxf->fc, time_base);
    if (!spf) {
        int remainder = (sample_rate.num * time_base.num) % (time_base.den * sample_rate.den);
        *sample_count = av_q2d(av_mul_q((AVRational){mxf->current_edit_unit, 1},
                                        av_mul_q(sample_rate, time_base)));
        if (remainder)
            av_log(mxf->fc, AV_LOG_WARNING,
                   "seeking detected on stream #%d with time base (%d/%d) and sample rate (%d/%d), audio pts won't be accurate.\n",
                   stream_index, time_base.num, time_base.den, sample_rate.num, sample_rate.den);
        return 0;
    }

    while (spf->samples_per_frame[size]) {
        total += spf->samples_per_frame[size];
        size++;
    }

    av_assert2(size);

    *sample_count = (mxf->current_edit_unit / size) * total;
    for (i = 0; i < mxf->current_edit_unit % size; i++) {
        *sample_count += spf->samples_per_frame[i];
    }

    return 0;
}
