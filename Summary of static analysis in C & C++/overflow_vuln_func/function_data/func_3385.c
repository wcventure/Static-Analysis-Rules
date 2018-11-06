float ff_rate_estimate_qscale(MpegEncContext *s, int dry_run)
{
    float q;
    int qmin, qmax;
    float br_compensation;
    double diff;
    double short_term_q;
    double fps;
    int picture_number = s->picture_number;
    int64_t wanted_bits;
    RateControlContext *rcc = &s->rc_context;
    AVCodecContext *a       = s->avctx;
    RateControlEntry local_rce, *rce;
    double bits;
    double rate_factor;
    int var;
    const int pict_type = s->pict_type;
    Picture * const pic = &s->current_picture;
    emms_c();

#if CONFIG_LIBXVID
    if ((s->flags & CODEC_FLAG_PASS2) &&
        s->avctx->rc_strategy == FF_RC_STRATEGY_XVID)
        return ff_xvid_rate_estimate_qscale(s, dry_run);
#endif

    get_qminmax(&qmin, &qmax, s, pict_type);

    fps = get_fps(s->avctx);
    /
    if (picture_number > 2 && !dry_run) {
        const int last_var = s->last_pict_type == AV_PICTURE_TYPE_I ? rcc->last_mb_var_sum
                                                                    : rcc->last_mc_mb_var_sum;
        av_assert1(s->frame_bits >= s->stuffing_bits);
        update_predictor(&rcc->pred[s->last_pict_type],
                         rcc->last_qscale,
                         sqrt(last_var),
                         s->frame_bits - s->stuffing_bits);
    }

    if (s->flags & CODEC_FLAG_PASS2) {
        assert(picture_number >= 0);
        if (picture_number >= rcc->num_entries) {
            av_log(s, AV_LOG_ERROR, "Input is longer than 2-pass log file\n");
            return -1;
        }
        rce         = &rcc->entry[picture_number];
        wanted_bits = rce->expected_bits;
    } else {
        Picture *dts_pic;
        rce = &local_rce;

        /
        if (s->pict_type == AV_PICTURE_TYPE_B || s->low_delay)
            dts_pic = s->current_picture_ptr;
        else
            dts_pic = s->last_picture_ptr;

        if (!dts_pic || dts_pic->f.pts == AV_NOPTS_VALUE)
            wanted_bits = (uint64_t)(s->bit_rate * (double)picture_number / fps);
        else
            wanted_bits = (uint64_t)(s->bit_rate * (double)dts_pic->f.pts / fps);
    }

    diff = s->total_bits - wanted_bits;
    br_compensation = (a->bit_rate_tolerance - diff) / a->bit_rate_tolerance;
    if (br_compensation <= 0.0)
        br_compensation = 0.001;

    var = pict_type == AV_PICTURE_TYPE_I ? pic->mb_var_sum : pic->mc_mb_var_sum;

    short_term_q = 0; /
    if (s->flags & CODEC_FLAG_PASS2) {
        if (pict_type != AV_PICTURE_TYPE_I)
            assert(pict_type == rce->new_pict_type);

        q = rce->new_qscale / br_compensation;
        av_dlog(s, "%f %f %f last:%d var:%d type:%d//\n", q, rce->new_qscale,
                br_compensation, s->frame_bits, var, pict_type);
    } else {
        rce->pict_type     =
        rce->new_pict_type = pict_type;
        rce->mc_mb_var_sum = pic->mc_mb_var_sum;
        rce->mb_var_sum    = pic->mb_var_sum;
        rce->qscale        = FF_QP2LAMBDA * 2;
        rce->f_code        = s->f_code;
        rce->b_code        = s->b_code;
        rce->misc_bits     = 1;

        bits = predict_size(&rcc->pred[pict_type], rce->qscale, sqrt(var));
        if (pict_type == AV_PICTURE_TYPE_I) {
            rce->i_count    = s->mb_num;
            rce->i_tex_bits = bits;
            rce->p_tex_bits = 0;
            rce->mv_bits    = 0;
        } else {
            rce->i_count    = 0;    // FIXME we do know this approx
            rce->i_tex_bits = 0;
            rce->p_tex_bits = bits * 0.9;
            rce->mv_bits    = bits * 0.1;
        }
        rcc->i_cplx_sum[pict_type]  += rce->i_tex_bits * rce->qscale;
        rcc->p_cplx_sum[pict_type]  += rce->p_tex_bits * rce->qscale;
        rcc->mv_bits_sum[pict_type] += rce->mv_bits;
        rcc->frame_count[pict_type]++;

        bits        = rce->i_tex_bits + rce->p_tex_bits;
        rate_factor = rcc->pass1_wanted_bits /
                      rcc->pass1_rc_eq_output_sum * br_compensation;

        q = get_qscale(s, rce, rate_factor, picture_number);
        if (q < 0)
            return -1;

        assert(q > 0.0);
        q = get_diff_limited_q(s, rce, q);
        assert(q > 0.0);

        // FIXME type dependent blur like in 2-pass
        if (pict_type == AV_PICTURE_TYPE_P || s->intra_only) {
            rcc->short_term_qsum   *= a->qblur;
            rcc->short_term_qcount *= a->qblur;

            rcc->short_term_qsum += q;
            rcc->short_term_qcount++;
            q = short_term_q = rcc->short_term_qsum / rcc->short_term_qcount;
        }
        assert(q > 0.0);

        q = modify_qscale(s, rce, q, picture_number);

        rcc->pass1_wanted_bits += s->bit_rate / fps;

        assert(q > 0.0);
    }

    if (s->avctx->debug & FF_DEBUG_RC) {
        av_log(s->avctx, AV_LOG_DEBUG,
               "%c qp:%d<%2.1f<%d %d want:%d total:%d comp:%f st_q:%2.2f "
               "size:%d var:%"PRId64"/%"PRId64" br:%d fps:%d\n",
               av_get_picture_type_char(pict_type),
               qmin, q, qmax, picture_number,
               (int)wanted_bits / 1000, (int)s->total_bits / 1000,
               br_compensation, short_term_q, s->frame_bits,
               pic->mb_var_sum, pic->mc_mb_var_sum,
               s->bit_rate / 1000, (int)fps);
    }

    if (q < qmin)
        q = qmin;
    else if (q > qmax)
        q = qmax;

    if (s->adaptive_quant)
        adaptive_quantization(s, q);
    else
        q = (int)(q + 0.5);

    if (!dry_run) {
        rcc->last_qscale        = q;
        rcc->last_mc_mb_var_sum = pic->mc_mb_var_sum;
        rcc->last_mb_var_sum    = pic->mb_var_sum;
    }
    return q;
}
