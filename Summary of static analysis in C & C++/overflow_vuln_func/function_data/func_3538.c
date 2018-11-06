static int decode_header(SnowContext *s){
    int plane_index, tmp;
    uint8_t kstate[32];

    memset(kstate, MID_STATE, sizeof(kstate));

    s->keyframe= get_rac(&s->c, kstate);
    if(s->keyframe || s->always_reset){
        ff_snow_reset_contexts(s);
        s->spatial_decomposition_type=
        s->qlog=
        s->qbias=
        s->mv_scale=
        s->block_max_depth= 0;
    }
    if(s->keyframe){
        GET_S(s->version, tmp <= 0U)
        s->always_reset= get_rac(&s->c, s->header_state);
        s->temporal_decomposition_type= get_symbol(&s->c, s->header_state, 0);
        s->temporal_decomposition_count= get_symbol(&s->c, s->header_state, 0);
        GET_S(s->spatial_decomposition_count, 0 < tmp && tmp <= MAX_DECOMPOSITIONS)
        s->colorspace_type= get_symbol(&s->c, s->header_state, 0);
        if (s->colorspace_type == 1) {
            s->avctx->pix_fmt= AV_PIX_FMT_GRAY8;
            s->nb_planes = 1;
        } else if(s->colorspace_type == 0) {
            s->chroma_h_shift= get_symbol(&s->c, s->header_state, 0);
            s->chroma_v_shift= get_symbol(&s->c, s->header_state, 0);

            if(s->chroma_h_shift == 1 && s->chroma_v_shift==1){
                s->avctx->pix_fmt= AV_PIX_FMT_YUV420P;
            }else if(s->chroma_h_shift == 0 && s->chroma_v_shift==0){
                s->avctx->pix_fmt= AV_PIX_FMT_YUV444P;
            }else if(s->chroma_h_shift == 2 && s->chroma_v_shift==2){
                s->avctx->pix_fmt= AV_PIX_FMT_YUV410P;
            } else {
                av_log(s, AV_LOG_ERROR, "unsupported color subsample mode %d %d\n", s->chroma_h_shift, s->chroma_v_shift);
                s->chroma_h_shift = s->chroma_v_shift = 1;
                s->avctx->pix_fmt= AV_PIX_FMT_YUV420P;
                return AVERROR_INVALIDDATA;
            }
            s->nb_planes = 3;
        } else {
            av_log(s, AV_LOG_ERROR, "unsupported color space\n");
            s->chroma_h_shift = s->chroma_v_shift = 1;
            s->avctx->pix_fmt= AV_PIX_FMT_YUV420P;
            return AVERROR_INVALIDDATA;
        }


        s->spatial_scalability= get_rac(&s->c, s->header_state);
//        s->rate_scalability= get_rac(&s->c, s->header_state);
        GET_S(s->max_ref_frames, tmp < (unsigned)MAX_REF_FRAMES)
        s->max_ref_frames++;

        decode_qlogs(s);
    }

    if(!s->keyframe){
        if(get_rac(&s->c, s->header_state)){
            for(plane_index=0; plane_index<FFMIN(s->nb_planes, 2); plane_index++){
                int htaps, i, sum=0;
                Plane *p= &s->plane[plane_index];
                p->diag_mc= get_rac(&s->c, s->header_state);
                htaps= get_symbol(&s->c, s->header_state, 0)*2 + 2;
                if((unsigned)htaps > HTAPS_MAX || htaps==0)
                    return AVERROR_INVALIDDATA;
                p->htaps= htaps;
                for(i= htaps/2; i; i--){
                    p->hcoeff[i]= get_symbol(&s->c, s->header_state, 0) * (1-2*(i&1));
                    sum += p->hcoeff[i];
                }
                p->hcoeff[0]= 32-sum;
            }
            s->plane[2].diag_mc= s->plane[1].diag_mc;
            s->plane[2].htaps  = s->plane[1].htaps;
            memcpy(s->plane[2].hcoeff, s->plane[1].hcoeff, sizeof(s->plane[1].hcoeff));
        }
        if(get_rac(&s->c, s->header_state)){
            GET_S(s->spatial_decomposition_count, 0 < tmp && tmp <= MAX_DECOMPOSITIONS)
            decode_qlogs(s);
        }
    }

    s->spatial_decomposition_type+= get_symbol(&s->c, s->header_state, 1);
    if(s->spatial_decomposition_type > 1U){
        av_log(s->avctx, AV_LOG_ERROR, "spatial_decomposition_type %d not supported\n", s->spatial_decomposition_type);
        return AVERROR_INVALIDDATA;
    }
    if(FFMIN(s->avctx-> width>>s->chroma_h_shift,
             s->avctx->height>>s->chroma_v_shift) >> (s->spatial_decomposition_count-1) <= 1){
        av_log(s->avctx, AV_LOG_ERROR, "spatial_decomposition_count %d too large for size\n", s->spatial_decomposition_count);
        return AVERROR_INVALIDDATA;
    }
    if (s->avctx->width > 65536-4) {
        av_log(s->avctx, AV_LOG_ERROR, "Width %d is too large\n", s->avctx->width);
        return AVERROR_INVALIDDATA;
    }


    s->qlog           += get_symbol(&s->c, s->header_state, 1);
    s->mv_scale       += get_symbol(&s->c, s->header_state, 1);
    s->qbias          += get_symbol(&s->c, s->header_state, 1);
    s->block_max_depth+= get_symbol(&s->c, s->header_state, 1);
    if(s->block_max_depth > 1 || s->block_max_depth < 0){
        av_log(s->avctx, AV_LOG_ERROR, "block_max_depth= %d is too large\n", s->block_max_depth);
        s->block_max_depth= 0;
        return AVERROR_INVALIDDATA;
    }
    if (FFABS(s->qbias) > 127) {
        av_log(s->avctx, AV_LOG_ERROR, "qbias %d is too large\n", s->qbias);
        s->qbias = 0;
        return AVERROR_INVALIDDATA;
    }

    return 0;
}
