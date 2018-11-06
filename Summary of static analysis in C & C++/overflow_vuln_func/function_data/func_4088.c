static void avoid_clipping(AACEncContext *s, SingleChannelElement *sce)
{
    int start, i, j, w;

    if (sce->ics.clip_avoidance_factor < 1.0f) {
        for (w = 0; w < sce->ics.num_windows; w++) {
            start = 0;
            for (i = 0; i < sce->ics.max_sfb; i++) {
                float *swb_coeffs = sce->coeffs + start + w*128;
                for (j = 0; j < sce->ics.swb_sizes[i]; j++)
                    swb_coeffs[j] *= sce->ics.clip_avoidance_factor;
                start += sce->ics.swb_sizes[i];
            }
        }
    }
}
