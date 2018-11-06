int RENAME(swri_resample)(ResampleContext *c, DELEM *dst, const DELEM *src, int *consumed, int src_size, int dst_size, int update_ctx){
    int dst_index, i;
    int index= c->index;
    int frac= c->frac;
    int dst_incr_frac= c->dst_incr % c->src_incr;
    int dst_incr=      c->dst_incr / c->src_incr;
    int compensation_distance= c->compensation_distance;

    av_assert1(c->filter_shift == FILTER_SHIFT);
    av_assert1(c->felem_size == sizeof(FELEM));

    if(compensation_distance == 0 && c->filter_length == 1 && c->phase_shift==0){
        int64_t index2= (1LL<<32)*c->frac/c->src_incr + (1LL<<32)*index;
        int64_t incr= (1LL<<32) * c->dst_incr / c->src_incr;
        int new_size = (src_size * (int64_t)c->src_incr - frac + c->dst_incr - 1) / c->dst_incr;

        dst_size= FFMIN(dst_size, new_size);

        for(dst_index=0; dst_index < dst_size; dst_index++){
            dst[dst_index] = src[index2>>32];
            index2 += incr;
        }
        index += dst_index * dst_incr;
        index += (frac + dst_index * (int64_t)dst_incr_frac) / c->src_incr;
        frac   = (frac + dst_index * (int64_t)dst_incr_frac) % c->src_incr;
        av_assert2(index >= 0);
        *consumed= index;
        index = 0;
    } else if (compensation_distance == 0 &&
               index >= 0 &&
               src_size*(int64_t)c->src_incr < (INT64_MAX >> (c->phase_shift+1))) {
        int64_t end_index = (1LL + src_size - c->filter_length) << c->phase_shift;
        int64_t delta_frac = (end_index - index) * c->src_incr - c->frac;
        int delta_n = (delta_frac + c->dst_incr - 1) / c->dst_incr;
        int n = FFMIN(dst_size, delta_n);
        int sample_index;

        if (!c->linear) {
            sample_index = index >> c->phase_shift;
            index &= c->phase_mask;
            for (dst_index = 0; dst_index < n; dst_index++) {
                FELEM *filter = ((FELEM *) c->filter_bank) + c->filter_alloc * index;

#ifdef COMMON_CORE
                COMMON_CORE
#else
                FELEM2 val=0;
                for (i = 0; i < c->filter_length; i++) {
                    val += src[sample_index + i] * (FELEM2)filter[i];
                }
                OUT(dst[dst_index], val);
#endif

                frac += dst_incr_frac;
                index += dst_incr;
                if (frac >= c->src_incr) {
                    frac -= c->src_incr;
                    index++;
                }
                sample_index += index >> c->phase_shift;
                index &= c->phase_mask;
            }
        } else {
            sample_index = index >> c->phase_shift;
            index &= c->phase_mask;
            for (dst_index = 0; dst_index < n; dst_index++) {
                FELEM *filter = ((FELEM *) c->filter_bank) + c->filter_alloc * index;
                FELEM2 val=0, v2 = 0;

#ifdef LINEAR_CORE
                LINEAR_CORE
#else
                for (i = 0; i < c->filter_length; i++) {
                    val += src[sample_index + i] * (FELEM2)filter[i];
                    v2  += src[sample_index + i] * (FELEM2)filter[i + c->filter_alloc];
                }
#endif
                val += (v2 - val) * (FELEML) frac / c->src_incr;
                OUT(dst[dst_index], val);

                frac += dst_incr_frac;
                index += dst_incr;
                if (frac >= c->src_incr) {
                    frac -= c->src_incr;
                    index++;
                }
                sample_index += index >> c->phase_shift;
                index &= c->phase_mask;
            }
        }

        *consumed = sample_index;
    } else {
        int sample_index = 0;
        for(dst_index=0; dst_index < dst_size; dst_index++){
            FELEM *filter;
            FELEM2 val=0;

            sample_index += index >> c->phase_shift;
            index &= c->phase_mask;
            filter = ((FELEM*)c->filter_bank) + c->filter_alloc*index;

            if(sample_index + c->filter_length > src_size || -sample_index >= src_size){
                break;
            }else if(sample_index < 0){
                for(i=0; i<c->filter_length; i++)
                    val += src[FFABS(sample_index + i)] * (FELEM2)filter[i];
                OUT(dst[dst_index], val);
            }else if(c->linear){
                FELEM2 v2=0;
#ifdef LINEAR_CORE
                LINEAR_CORE
#else
                for(i=0; i<c->filter_length; i++){
                    val += src[sample_index + i] * (FELEM2)filter[i];
                    v2  += src[sample_index + i] * (FELEM2)filter[i + c->filter_alloc];
                }
#endif
                val+=(v2-val)*(FELEML)frac / c->src_incr;
                OUT(dst[dst_index], val);
            }else{
#ifdef COMMON_CORE
                COMMON_CORE
#else
                for(i=0; i<c->filter_length; i++){
                    val += src[sample_index + i] * (FELEM2)filter[i];
                }
                OUT(dst[dst_index], val);
#endif
            }

            frac += dst_incr_frac;
            index += dst_incr;
            if(frac >= c->src_incr){
                frac -= c->src_incr;
                index++;
            }

            if(dst_index + 1 == compensation_distance){
                compensation_distance= 0;
                dst_incr_frac= c->ideal_dst_incr % c->src_incr;
                dst_incr=      c->ideal_dst_incr / c->src_incr;
            }
        }
        *consumed= FFMAX(sample_index, 0);
        index += FFMIN(sample_index, 0) << c->phase_shift;

        if(compensation_distance){
            compensation_distance -= dst_index;
            av_assert1(compensation_distance > 0);
        }
    }

    if(update_ctx){
        c->frac= frac;
        c->index= index;
        c->dst_incr= dst_incr_frac + c->src_incr*dst_incr;
        c->compensation_distance= compensation_distance;
    }

    return dst_index;
}
