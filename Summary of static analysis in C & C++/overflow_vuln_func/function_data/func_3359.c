static inline void silk_stabilize_lsf(int16_t nlsf[16], int order, const uint16_t min_delta[17])
{
    int pass, i;
    for (pass = 0; pass < 20; pass++) {
        int k, min_diff = 0;
        for (i = 0; i < order+1; i++) {
            int low  = i != 0     ? nlsf[i-1] : 0;
            int high = i != order ? nlsf[i]   : 32768;
            int diff = (high - low) - (min_delta[i]);

            if (diff < min_diff) {
                min_diff = diff;
                k = i;

                if (pass == 20)
                    break;
            }
        }
        if (min_diff == 0) /
            return;

        /
        if (k == 0) {
            /
            nlsf[0] = min_delta[0];
        } else if (k == order) {
            /
            nlsf[order-1] = 32768 - min_delta[order];
        } else {
            /
            int min_center = 0, max_center = 32768, center_val;

            /
            for (i = 0; i < k; i++)
                min_center += min_delta[i];
            min_center += min_delta[k] >> 1;

            /
            for (i = order; i > k; i--)
                max_center -= min_delta[i];
            max_center -= min_delta[k] >> 1;

            /
            center_val = nlsf[k - 1] + nlsf[k];
            center_val = (center_val >> 1) + (center_val & 1); // rounded divide by 2
            center_val = FFMIN(max_center, FFMAX(min_center, center_val));

            nlsf[k - 1] = center_val - (min_delta[k] >> 1);
            nlsf[k]     = nlsf[k - 1] + min_delta[k];
        }
    }

    /

    /
    for (i = 1; i < order; i++) {
        int j, value = nlsf[i];
        for (j = i - 1; j >= 0 && nlsf[j] > value; j--)
            nlsf[j + 1] = nlsf[j];
        nlsf[j + 1] = value;
    }

    /
    if (nlsf[0] < min_delta[0])
        nlsf[0] = min_delta[0];
    for (i = 1; i < order; i++)
        if (nlsf[i] < nlsf[i - 1] + min_delta[i])
            nlsf[i] = nlsf[i - 1] + min_delta[i];

    /
    if (nlsf[order-1] > 32768 - min_delta[order])
        nlsf[order-1] = 32768 - min_delta[order];
    for (i = order-2; i >= 0; i--)
        if (nlsf[i] > nlsf[i + 1] - min_delta[i+1])
            nlsf[i] = nlsf[i + 1] - min_delta[i+1];

    return;
}
