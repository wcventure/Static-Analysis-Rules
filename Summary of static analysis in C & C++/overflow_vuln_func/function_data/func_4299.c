static ResampleContext *resample_init(ResampleContext *c, int out_rate, int in_rate, int filter_size, int phase_shift, int linear,
                                    double cutoff0, enum AVSampleFormat format, enum SwrFilterType filter_type, int kaiser_beta,
                                    double precision, int cheby){
    double cutoff = cutoff0? cutoff0 : 0.97;
    double factor= FFMIN(out_rate * cutoff / in_rate, 1.0);
    int phase_count= 1<<phase_shift;

    if (!c || c->phase_shift != phase_shift || c->linear!=linear || c->factor != factor
           || c->filter_length != FFMAX((int)ceil(filter_size/factor), 1) || c->format != format
           || c->filter_type != filter_type || c->kaiser_beta != kaiser_beta) {
        c = av_mallocz(sizeof(*c));
        if (!c)
            return NULL;

        c->format= format;

        c->felem_size= av_get_bytes_per_sample(c->format);

        switch(c->format){
        case AV_SAMPLE_FMT_S16P:
            c->filter_shift = 15;
            break;
        case AV_SAMPLE_FMT_S32P:
            c->filter_shift = 30;
            break;
        case AV_SAMPLE_FMT_FLTP:
        case AV_SAMPLE_FMT_DBLP:
            c->filter_shift = 0;
            break;
        default:
            av_log(NULL, AV_LOG_ERROR, "Unsupported sample format\n");
            av_assert0(0);
        }

        c->phase_shift   = phase_shift;
        c->phase_mask    = phase_count - 1;
        c->linear        = linear;
        c->factor        = factor;
        c->filter_length = FFMAX((int)ceil(filter_size/factor), 1);
        c->filter_alloc  = FFALIGN(c->filter_length, 8);
        c->filter_bank   = av_mallocz(c->filter_alloc*(phase_count+1)*c->felem_size);
        c->filter_type   = filter_type;
        c->kaiser_beta   = kaiser_beta;
        if (!c->filter_bank)
            goto error;
        if (build_filter(c, (void*)c->filter_bank, factor, c->filter_length, c->filter_alloc, phase_count, 1<<c->filter_shift, filter_type, kaiser_beta))
            goto error;
        memcpy(c->filter_bank + (c->filter_alloc*phase_count+1)*c->felem_size, c->filter_bank, (c->filter_alloc-1)*c->felem_size);
        memcpy(c->filter_bank + (c->filter_alloc*phase_count  )*c->felem_size, c->filter_bank + (c->filter_alloc - 1)*c->felem_size, c->felem_size);
    }

    c->compensation_distance= 0;
    if(!av_reduce(&c->src_incr, &c->dst_incr, out_rate, in_rate * (int64_t)phase_count, INT32_MAX/2))
        goto error;
    c->ideal_dst_incr= c->dst_incr;

    c->index= -phase_count*((c->filter_length-1)/2);
    c->frac= 0;

    return c;
error:
    av_free(c->filter_bank);
    av_free(c);
    return NULL;
}
