static void encode_picture(MpegEncContext *s, int picture_number)
{
    int i;
    int bits;

    s->picture_number = picture_number;
    
    /
    s->me.mb_var_sum_temp    =
    s->me.mc_mb_var_sum_temp = 0;

    /
    // RAL: Condition added for MPEG1VIDEO
    if (s->codec_id == CODEC_ID_MPEG1VIDEO || s->codec_id == CODEC_ID_MPEG2VIDEO || (s->h263_pred && !s->h263_msmpeg4))
        ff_set_mpeg4_time(s, s->picture_number);  //FIXME rename and use has_b_frames or similar
        
    s->me.scene_change_score=0;
    
//    s->lambda= s->current_picture_ptr->quality; //FIXME qscale / ... stuff for ME ratedistoration
    
    if(s->pict_type==I_TYPE){
        if(s->msmpeg4_version >= 3) s->no_rounding=1;
        else                        s->no_rounding=0;
    }else if(s->pict_type!=B_TYPE){
        if(s->flipflop_rounding || s->codec_id == CODEC_ID_H263P || s->codec_id == CODEC_ID_MPEG4)
            s->no_rounding ^= 1;          
    }
    
    s->mb_intra=0; //for the rate distortion & bit compare functions
    for(i=1; i<s->avctx->thread_count; i++){
        ff_update_duplicate_context(s->thread_context[i], s);
    }

    ff_init_me(s);

    /
    if(s->pict_type != I_TYPE){
        s->lambda = (s->lambda * s->avctx->me_penalty_compensation + 128)>>8;
        s->lambda2= (s->lambda2* s->avctx->me_penalty_compensation + 128)>>8;
        if(s->pict_type != B_TYPE && s->avctx->me_threshold==0){
            if((s->avctx->pre_me && s->last_non_b_pict_type==I_TYPE) || s->avctx->pre_me==2){
                s->avctx->execute(s->avctx, pre_estimate_motion_thread, (void**)&(s->thread_context[0]), NULL, s->avctx->thread_count);
            }
        }

        s->avctx->execute(s->avctx, estimate_motion_thread, (void**)&(s->thread_context[0]), NULL, s->avctx->thread_count);
    }else /{
        /
        for(i=0; i<s->mb_stride*s->mb_height; i++)
            s->mb_type[i]= CANDIDATE_MB_TYPE_INTRA;
        
        if(!s->fixed_qscale){
            /
            s->avctx->execute(s->avctx, mb_var_thread, (void**)&(s->thread_context[0]), NULL, s->avctx->thread_count);
        }
    }
    for(i=1; i<s->avctx->thread_count; i++){
        merge_context_after_me(s, s->thread_context[i]);
    }
    s->current_picture.mc_mb_var_sum= s->current_picture_ptr->mc_mb_var_sum= s->me.mc_mb_var_sum_temp;
    s->current_picture.   mb_var_sum= s->current_picture_ptr->   mb_var_sum= s->me.   mb_var_sum_temp;
    emms_c();

    if(s->me.scene_change_score > s->avctx->scenechange_threshold && s->pict_type == P_TYPE){
        s->pict_type= I_TYPE;
        for(i=0; i<s->mb_stride*s->mb_height; i++)
            s->mb_type[i]= CANDIDATE_MB_TYPE_INTRA;
//printf("Scene change detected, encoding as I Frame %d %d\n", s->current_picture.mb_var_sum, s->current_picture.mc_mb_var_sum);
    }

    if(!s->umvplus){
        if(s->pict_type==P_TYPE || s->pict_type==S_TYPE) {
            s->f_code= ff_get_best_fcode(s, s->p_mv_table, CANDIDATE_MB_TYPE_INTER);

            if(s->flags & CODEC_FLAG_INTERLACED_ME){
                int a,b;
                a= ff_get_best_fcode(s, s->p_field_mv_table[0][0], CANDIDATE_MB_TYPE_INTER_I); //FIXME field_select
                b= ff_get_best_fcode(s, s->p_field_mv_table[1][1], CANDIDATE_MB_TYPE_INTER_I);
                s->f_code= FFMAX(s->f_code, FFMAX(a,b));
            }
                    
            ff_fix_long_p_mvs(s);
            ff_fix_long_mvs(s, NULL, 0, s->p_mv_table, s->f_code, CANDIDATE_MB_TYPE_INTER, 0);
            if(s->flags & CODEC_FLAG_INTERLACED_ME){
                int j;
                for(i=0; i<2; i++){
                    for(j=0; j<2; j++)
                        ff_fix_long_mvs(s, s->p_field_select_table[i], j, 
                                        s->p_field_mv_table[i][j], s->f_code, CANDIDATE_MB_TYPE_INTER_I, 0);
                }
            }
        }

        if(s->pict_type==B_TYPE){
            int a, b;

            a = ff_get_best_fcode(s, s->b_forw_mv_table, CANDIDATE_MB_TYPE_FORWARD);
            b = ff_get_best_fcode(s, s->b_bidir_forw_mv_table, CANDIDATE_MB_TYPE_BIDIR);
            s->f_code = FFMAX(a, b);

            a = ff_get_best_fcode(s, s->b_back_mv_table, CANDIDATE_MB_TYPE_BACKWARD);
            b = ff_get_best_fcode(s, s->b_bidir_back_mv_table, CANDIDATE_MB_TYPE_BIDIR);
            s->b_code = FFMAX(a, b);

            ff_fix_long_mvs(s, NULL, 0, s->b_forw_mv_table, s->f_code, CANDIDATE_MB_TYPE_FORWARD, 1);
            ff_fix_long_mvs(s, NULL, 0, s->b_back_mv_table, s->b_code, CANDIDATE_MB_TYPE_BACKWARD, 1);
            ff_fix_long_mvs(s, NULL, 0, s->b_bidir_forw_mv_table, s->f_code, CANDIDATE_MB_TYPE_BIDIR, 1);
            ff_fix_long_mvs(s, NULL, 0, s->b_bidir_back_mv_table, s->b_code, CANDIDATE_MB_TYPE_BIDIR, 1);
            if(s->flags & CODEC_FLAG_INTERLACED_ME){
                int dir, j;
                for(dir=0; dir<2; dir++){
                    for(i=0; i<2; i++){
                        for(j=0; j<2; j++){
                            int type= dir ? (CANDIDATE_MB_TYPE_BACKWARD_I|CANDIDATE_MB_TYPE_BIDIR_I) 
                                          : (CANDIDATE_MB_TYPE_FORWARD_I |CANDIDATE_MB_TYPE_BIDIR_I);
                            ff_fix_long_mvs(s, s->b_field_select_table[dir][i], j, 
                                            s->b_field_mv_table[dir][i][j], dir ? s->b_code : s->f_code, type, 1);
                        }
                    }
                }
            }
        }
    }

    if (!s->fixed_qscale) 
        s->current_picture.quality = ff_rate_estimate_qscale(s); //FIXME pic_ptr

    if(s->adaptive_quant){
        switch(s->codec_id){
        case CODEC_ID_MPEG4:
            ff_clean_mpeg4_qscales(s);
            break;
        case CODEC_ID_H263:
        case CODEC_ID_H263P:
        case CODEC_ID_FLV1:
            ff_clean_h263_qscales(s);
            break;
        }

        s->lambda= s->lambda_table[0];
        //FIXME broken
    }else
        s->lambda= s->current_picture.quality;
//printf("%d %d\n", s->avctx->global_quality, s->current_picture.quality);
    update_qscale(s);
    
    if(s->qscale < 3 && s->max_qcoeff<=128 && s->pict_type==I_TYPE && !(s->flags & CODEC_FLAG_QSCALE)) 
        s->qscale= 3; //reduce clipping problems
        
    if (s->out_format == FMT_MJPEG) {
        /
        s->intra_matrix[0] = ff_mpeg1_default_intra_matrix[0];
        for(i=1;i<64;i++){
            int j= s->dsp.idct_permutation[i];

            s->intra_matrix[j] = clip_uint8((ff_mpeg1_default_intra_matrix[i] * s->qscale) >> 3);
        }
        convert_matrix(&s->dsp, s->q_intra_matrix, s->q_intra_matrix16, 
                       s->intra_matrix, s->intra_quant_bias, 8, 8, 1);
        s->qscale= 8;
    }
    
    //FIXME var duplication
    s->current_picture_ptr->key_frame=
    s->current_picture.key_frame= s->pict_type == I_TYPE; //FIXME pic_ptr
    s->current_picture_ptr->pict_type=
    s->current_picture.pict_type= s->pict_type;

    if(s->current_picture.key_frame)
        s->picture_in_gop_number=0;

    s->last_bits= put_bits_count(&s->pb);
    switch(s->out_format) {
    case FMT_MJPEG:
        mjpeg_picture_header(s);
        break;
#ifdef CONFIG_H261_ENCODER
    case FMT_H261:
        ff_h261_encode_picture_header(s, picture_number);
        break;
#endif
    case FMT_H263:
        if (s->codec_id == CODEC_ID_WMV2) 
            ff_wmv2_encode_picture_header(s, picture_number);
        else if (s->h263_msmpeg4) 
            msmpeg4_encode_picture_header(s, picture_number);
        else if (s->h263_pred)
            mpeg4_encode_picture_header(s, picture_number);
#ifdef CONFIG_RV10_ENCODER
        else if (s->codec_id == CODEC_ID_RV10) 
            rv10_encode_picture_header(s, picture_number);
#endif
#ifdef CONFIG_RV20_ENCODER
        else if (s->codec_id == CODEC_ID_RV20) 
            rv20_encode_picture_header(s, picture_number);
#endif
        else if (s->codec_id == CODEC_ID_FLV1)
            ff_flv_encode_picture_header(s, picture_number);
        else
            h263_encode_picture_header(s, picture_number);
        break;
    case FMT_MPEG1:
        mpeg1_encode_picture_header(s, picture_number);
        break;
    case FMT_H264:
        break;
    default:
        assert(0);
    }
    bits= put_bits_count(&s->pb);
    s->header_bits= bits - s->last_bits;
        
    for(i=1; i<s->avctx->thread_count; i++){
        update_duplicate_context_after_me(s->thread_context[i], s);
    }
    s->avctx->execute(s->avctx, encode_thread, (void**)&(s->thread_context[0]), NULL, s->avctx->thread_count);
    for(i=1; i<s->avctx->thread_count; i++){
        merge_context_after_encode(s, s->thread_context[i]);
    }
    emms_c();
}
