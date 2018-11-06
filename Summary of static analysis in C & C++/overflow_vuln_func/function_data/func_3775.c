static void choose_sample_rate(AVStream *st, AVCodec *codec)
{
    if (codec && codec->supported_samplerates) {
        const int *p  = codec->supported_samplerates;
        int best      = 0;
        int best_dist = INT_MAX;
        for (; *p; p++) {
            int dist = abs(st->codec->sample_rate - *p);
            if (dist < best_dist) {
                best_dist = dist;
                best      = *p;
            }
        }
        if (best_dist) {
            av_log(st->codec, AV_LOG_WARNING, "Requested sampling rate unsupported using closest supported (%d)\n", best);
        }
        st->codec->sample_rate = best;
    }
}
