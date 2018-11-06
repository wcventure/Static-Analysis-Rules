int ff_celp_lp_synthesis_filter(int16_t *out, const int16_t *filter_coeffs,
                                const int16_t *in, int buffer_length,
                                int filter_length, int stop_on_overflow,
                                int shift, int rounder)
{
    int i,n;

    for (n = 0; n < buffer_length; n++) {
        int sum = -rounder, sum1;
        for (i = 1; i <= filter_length; i++)
            sum += filter_coeffs[i-1] * out[n-i];

        sum1 = ((-sum >> 12) + in[n]) >> shift;
        sum  = av_clip_int16(sum1);

        if (stop_on_overflow && sum != sum1)
            return 1;

        out[n] = sum;
    }

    return 0;
}
