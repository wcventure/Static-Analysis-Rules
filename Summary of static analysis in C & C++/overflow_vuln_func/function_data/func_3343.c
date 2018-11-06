static int window(venc_context_t * venc, signed short * audio, int samples) {
    int i, j, channel;
    const float * win = venc->win[0];
    int window_len = 1 << (venc->blocksize[0] - 1);
    float n = (float)(1 << venc->blocksize[0]) / 4.;
    // FIXME use dsp

    if (!venc->have_saved && !samples) return 0;

    if (venc->have_saved) {
        for (channel = 0; channel < venc->channels; channel++) {
            memcpy(venc->samples + channel*window_len*2, venc->saved + channel*window_len, sizeof(float)*window_len);
        }
    } else {
        for (channel = 0; channel < venc->channels; channel++) {
            memset(venc->samples + channel*window_len*2, 0, sizeof(float)*window_len);
        }
    }

    if (samples) {
        for (channel = 0; channel < venc->channels; channel++) {
            float * offset = venc->samples + channel*window_len*2 + window_len;
            j = channel;
            for (i = 0; i < samples; i++, j += venc->channels)
                offset[i] = audio[j] / 32768. * win[window_len - i] / n;
        }
    } else {
        for (channel = 0; channel < venc->channels; channel++) {
            memset(venc->samples + channel*window_len*2 + window_len, 0, sizeof(float)*window_len);
        }
    }

    for (channel = 0; channel < venc->channels; channel++) {
        ff_mdct_calc(&venc->mdct[0], venc->coeffs + channel*window_len, venc->samples + channel*window_len*2, venc->floor/);
    }

    if (samples) {
        for (channel = 0; channel < venc->channels; channel++) {
            float * offset = venc->saved + channel*window_len;
            j = channel;
            for (i = 0; i < samples; i++, j += venc->channels)
                offset[i] = audio[j] / 32768. * win[i] / n;
        }
        venc->have_saved = 1;
    } else {
        venc->have_saved = 0;
    }
    return 1;
}
