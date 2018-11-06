static int RENAME(resample_common)(ResampleContext *c,
                                   void *dest, const void *source,
                                   int n, int update_ctx)
{
    DELEM *dst = dest;
    const DELEM *src = source;
    int dst_index;
    int index= c->index;
    int frac= c->frac;
    int sample_index = 0;

    while (index >= c->phase_count) {
        sample_index++;
        index -= c->phase_count;
    }

    for (dst_index = 0; dst_index < n; dst_index++) {
        FELEM *filter = ((FELEM *) c->filter_bank) + c->filter_alloc * index;

        FELEM2 val= FOFFSET;
        int i;
        for (i = 0; i < c->filter_length; i++) {
            val += src[sample_index + i] * (FELEM2)filter[i];
        }
        OUT(dst[dst_index], val);

        frac  += c->dst_incr_mod;
        index += c->dst_incr_div;
        if (frac >= c->src_incr) {
            frac -= c->src_incr;
            index++;
        }

        while (index >= c->phase_count) {
            sample_index++;
            index -= c->phase_count;
        }
    }

    if(update_ctx){
        c->frac= frac;
        c->index= index;
    }

    return sample_index;
}
