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
            int i;
            const int *sample_rates = codec->supported_samplerates;
            av_log(st->codec, AV_LOG_WARNING,
                   "Requested sampling rate (%dHz) unsupported, using %dHz instead\n"
                   "Available sampling rates for %s:",
                   st->codec->sample_rate, best, codec->name);
            for (i = 0; sample_rates[i]; i++) {
                if (!sample_rates[i + 1]) av_log(st->codec, AV_LOG_WARNING, " and");
                else if (i)               av_log(st->codec, AV_LOG_WARNING, ",");
                av_log(st->codec, AV_LOG_WARNING, " %d", sample_rates[i]);
            }
            av_log(st->codec, AV_LOG_WARNING, ".\n");
        }
        st->codec->sample_rate = best;
    }
}
