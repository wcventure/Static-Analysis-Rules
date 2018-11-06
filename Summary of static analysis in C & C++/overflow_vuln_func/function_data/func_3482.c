void ff_aac_update_ltp(AACEncContext *s, SingleChannelElement *sce)
{
    int i, j, lag;
    float corr, s0, s1, max_corr = 0.0f;
    float *samples = &s->planar_samples[s->cur_channel][1024];
    float *pred_signal = &sce->ltp_state[0];
    int samples_num = 2048;

    if (s->profile != FF_PROFILE_AAC_LTP)
        return;

    /
    for (i = 0; i < samples_num; i++) {
        s0 = s1 = 0.0f;
        for (j = 0; j < samples_num; j++) {
            if (j + 1024 < i)
                continue;
            s0 += samples[j]*pred_signal[j-i+1024];
            s1 += pred_signal[j-i+1024]*pred_signal[j-i+1024];
        }
        corr = s1 > 0.0f ? s0/sqrt(s1) : 0.0f;
        if (corr > max_corr) {
            max_corr = corr;
            lag = i;
        }
    }
    lag = av_clip(lag, 0, 2048); /

    if (!lag) {
        sce->ics.ltp.lag = lag;
        return;
    }

    s0 = s1 = 0.0f;
    for (i = 0; i < lag; i++) {
        s0 += samples[i];
        s1 += pred_signal[i-lag+1024];
    }

    sce->ics.ltp.coef_idx = quant_array_idx(s0/s1, ltp_coef, 8);
    sce->ics.ltp.coef     = ltp_coef[sce->ics.ltp.coef_idx];

    /
    if (lag < 1024)
        samples_num = lag + 1024;
    for (i = 0; i < samples_num; i++)
        pred_signal[i+1024] = sce->ics.ltp.coef*pred_signal[i-lag+1024];
    memset(&pred_signal[samples_num], 0, (2048 - samples_num)*sizeof(float));

    sce->ics.ltp.lag = lag;
}
