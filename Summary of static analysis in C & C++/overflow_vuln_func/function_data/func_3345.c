int MPV_encode_init(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;
    int i, dummy;
    int chroma_h_shift, chroma_v_shift;
    
    MPV_encode_defaults(s);

    avctx->pix_fmt = PIX_FMT_YUV420P; // FIXME

    s->bit_rate = avctx->bit_rate;
    s->width = avctx->width;
    s->height = avctx->height;
    if(avctx->gop_size > 600){
	av_log(avctx, AV_LOG_ERROR, "Warning keyframe interval too large! reducing it ...\n");
        avctx->gop_size=600;
    }
    s->gop_size = avctx->gop_size;
    s->avctx = avctx;
    s->flags= avctx->flags;
    s->flags2= avctx->flags2;
    s->max_b_frames= avctx->max_b_frames;
    s->codec_id= avctx->codec->id;
    s->luma_elim_threshold  = avctx->luma_elim_threshold;
    s->chroma_elim_threshold= avctx->chroma_elim_threshold;
    s->strict_std_compliance= avctx->strict_std_compliance;
    s->data_partitioning= avctx->flags & CODEC_FLAG_PART;
    s->quarter_sample= (avctx->flags & CODEC_FLAG_QPEL)!=0;
    s->mpeg_quant= avctx->mpeg_quant;
    s->rtp_mode= !!avctx->rtp_payload_size;
    s->intra_dc_precision= avctx->intra_dc_precision;
    s->user_specified_pts = AV_NOPTS_VALUE;

    if (s->gop_size <= 1) {
        s->intra_only = 1;
        s->gop_size = 12;
    } else {
        s->intra_only = 0;
    }

    s->me_method = avctx->me_method;

    /
    s->fixed_qscale = !!(avctx->flags & CODEC_FLAG_QSCALE);
    
    s->adaptive_quant= (   s->avctx->lumi_masking
                        || s->avctx->dark_masking
                        || s->avctx->temporal_cplx_masking 
                        || s->avctx->spatial_cplx_masking
                        || s->avctx->p_masking
                        || (s->flags&CODEC_FLAG_QP_RD))
                       && !s->fixed_qscale;
    
    s->obmc= !!(s->flags & CODEC_FLAG_OBMC);
    s->loop_filter= !!(s->flags & CODEC_FLAG_LOOP_FILTER);
    s->alternate_scan= !!(s->flags & CODEC_FLAG_ALT_SCAN);

    if(avctx->rc_max_rate && !avctx->rc_buffer_size){
        av_log(avctx, AV_LOG_ERROR, "a vbv buffer size is needed, for encoding with a maximum bitrate\n");
        return -1;
    }    

    if(avctx->rc_min_rate && avctx->rc_max_rate != avctx->rc_min_rate){
        av_log(avctx, AV_LOG_INFO, "Warning min_rate > 0 but min_rate != max_rate isnt recommanded!\n");
    }
    
    if(avctx->rc_min_rate && avctx->rc_min_rate > avctx->bit_rate){
        av_log(avctx, AV_LOG_INFO, "bitrate below min bitrate\n");
        return -1;
    }
    
    if(avctx->rc_max_rate && avctx->rc_max_rate < avctx->bit_rate){
        av_log(avctx, AV_LOG_INFO, "bitrate above max bitrate\n");
        return -1;
    }
        
    if(   s->avctx->rc_max_rate && s->avctx->rc_min_rate == s->avctx->rc_max_rate 
       && (s->codec_id == CODEC_ID_MPEG1VIDEO || s->codec_id == CODEC_ID_MPEG2VIDEO)
       && 90000LL * (avctx->rc_buffer_size-1) > s->avctx->rc_max_rate*0xFFFFLL){
        
        av_log(avctx, AV_LOG_INFO, "Warning vbv_delay will be set to 0xFFFF (=VBR) as the specified vbv buffer is too large for the given bitrate!\n");
    }
       
    if((s->flags & CODEC_FLAG_4MV) && s->codec_id != CODEC_ID_MPEG4 
       && s->codec_id != CODEC_ID_H263 && s->codec_id != CODEC_ID_H263P && s->codec_id != CODEC_ID_FLV1){
        av_log(avctx, AV_LOG_ERROR, "4MV not supported by codec\n");
        return -1;
    }
        
    if(s->obmc && s->avctx->mb_decision != FF_MB_DECISION_SIMPLE){
        av_log(avctx, AV_LOG_ERROR, "OBMC is only supported with simple mb decission\n");
        return -1;
    }
    
    if(s->obmc && s->codec_id != CODEC_ID_H263 && s->codec_id != CODEC_ID_H263P){
        av_log(avctx, AV_LOG_ERROR, "OBMC is only supported with H263(+)\n");
        return -1;
    }
    
    if(s->quarter_sample && s->codec_id != CODEC_ID_MPEG4){
        av_log(avctx, AV_LOG_ERROR, "qpel not supported by codec\n");
        return -1;
    }

    if(s->data_partitioning && s->codec_id != CODEC_ID_MPEG4){
        av_log(avctx, AV_LOG_ERROR, "data partitioning not supported by codec\n");
        return -1;
    }
    
    if(s->max_b_frames && s->codec_id != CODEC_ID_MPEG4 && s->codec_id != CODEC_ID_MPEG1VIDEO && s->codec_id != CODEC_ID_MPEG2VIDEO){
        av_log(avctx, AV_LOG_ERROR, "b frames not supported by codec\n");
        return -1;
    }

    if((s->flags & (CODEC_FLAG_INTERLACED_DCT|CODEC_FLAG_INTERLACED_ME|CODEC_FLAG_ALT_SCAN)) 
       && s->codec_id != CODEC_ID_MPEG4 && s->codec_id != CODEC_ID_MPEG2VIDEO){
        av_log(avctx, AV_LOG_ERROR, "interlacing not supported by codec\n");
        return -1;
    }
        
    if(s->mpeg_quant && s->codec_id != CODEC_ID_MPEG4){ //FIXME mpeg2 uses that too
        av_log(avctx, AV_LOG_ERROR, "mpeg2 style quantization not supporetd by codec\n");
        return -1;
    }
        
    if((s->flags & CODEC_FLAG_CBP_RD) && !(s->flags & CODEC_FLAG_TRELLIS_QUANT)){
        av_log(avctx, AV_LOG_ERROR, "CBP RD needs trellis quant\n");
        return -1;
    }

    if((s->flags & CODEC_FLAG_QP_RD) && s->avctx->mb_decision != FF_MB_DECISION_RD){
        av_log(avctx, AV_LOG_ERROR, "QP RD needs mbd=2\n");
        return -1;
    }
    
    if(s->avctx->scenechange_threshold < 1000000000 && (s->flags & CODEC_FLAG_CLOSED_GOP)){
        av_log(avctx, AV_LOG_ERROR, "closed gop with scene change detection arent supported yet\n");
        return -1;
    }
    
    if(s->avctx->thread_count > 1 && s->codec_id != CODEC_ID_MPEG4 
       && s->codec_id != CODEC_ID_MPEG1VIDEO && s->codec_id != CODEC_ID_MPEG2VIDEO 
       && (s->codec_id != CODEC_ID_H263P || !(s->flags & CODEC_FLAG_H263P_SLICE_STRUCT))){
        av_log(avctx, AV_LOG_ERROR, "multi threaded encoding not supported by codec\n");
        return -1;
    }
    
    if(s->avctx->thread_count > 1)
        s->rtp_mode= 1;

    i= ff_gcd(avctx->frame_rate, avctx->frame_rate_base);
    if(i > 1){
        av_log(avctx, AV_LOG_INFO, "removing common factors from framerate\n");
        avctx->frame_rate /= i;
        avctx->frame_rate_base /= i;
//        return -1;
    }
    
    if(s->codec_id==CODEC_ID_MJPEG){
        s->intra_quant_bias= 1<<(QUANT_BIAS_SHIFT-1); //(a + x/2)/x
        s->inter_quant_bias= 0;
    }else if(s->mpeg_quant || s->codec_id==CODEC_ID_MPEG1VIDEO || s->codec_id==CODEC_ID_MPEG2VIDEO){
        s->intra_quant_bias= 3<<(QUANT_BIAS_SHIFT-3); //(a + x*3/8)/x
        s->inter_quant_bias= 0;
    }else{
        s->intra_quant_bias=0;
        s->inter_quant_bias=-(1<<(QUANT_BIAS_SHIFT-2)); //(a - x/4)/x
    }
    
    if(avctx->intra_quant_bias != FF_DEFAULT_QUANT_BIAS)
        s->intra_quant_bias= avctx->intra_quant_bias;
    if(avctx->inter_quant_bias != FF_DEFAULT_QUANT_BIAS)
        s->inter_quant_bias= avctx->inter_quant_bias;
        
    avcodec_get_chroma_sub_sample(avctx->pix_fmt, &chroma_h_shift, &chroma_v_shift);

    av_reduce(&s->time_increment_resolution, &dummy, s->avctx->frame_rate, s->avctx->frame_rate_base, (1<<16)-1);
    s->time_increment_bits = av_log2(s->time_increment_resolution - 1) + 1;

    switch(avctx->codec->id) {
    case CODEC_ID_MPEG1VIDEO:
        s->out_format = FMT_MPEG1;
        s->low_delay= 0; //s->max_b_frames ? 0 : 1;
        avctx->delay= s->low_delay ? 0 : (s->max_b_frames + 1);
        break;
    case CODEC_ID_MPEG2VIDEO:
        s->out_format = FMT_MPEG1;
        s->low_delay= 0; //s->max_b_frames ? 0 : 1;
        avctx->delay= s->low_delay ? 0 : (s->max_b_frames + 1);
        s->rtp_mode= 1;
        break;
    case CODEC_ID_LJPEG:
    case CODEC_ID_MJPEG:
        s->out_format = FMT_MJPEG;
        s->intra_only = 1; /
        s->mjpeg_write_tables = 1; /
	s->mjpeg_data_only_frames = 0; /
        s->mjpeg_vsample[0] = 1<<chroma_v_shift;
        s->mjpeg_vsample[1] = 1;
        s->mjpeg_vsample[2] = 1; 
        s->mjpeg_hsample[0] = 1<<chroma_h_shift;
        s->mjpeg_hsample[1] = 1; 
        s->mjpeg_hsample[2] = 1; 
        if (mjpeg_init(s) < 0)
            return -1;
        avctx->delay=0;
        s->low_delay=1;
        break;
#ifdef CONFIG_RISKY
    case CODEC_ID_H261:
        s->out_format = FMT_H261;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_H263:
        if (h263_get_picture_format(s->width, s->height) == 7) {
            av_log(avctx, AV_LOG_INFO, "Input picture size isn't suitable for h263 codec! try h263+\n");
            return -1;
        }
        s->out_format = FMT_H263;
	s->obmc= (avctx->flags & CODEC_FLAG_OBMC) ? 1:0;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_H263P:
        s->out_format = FMT_H263;
        s->h263_plus = 1;
	/
        s->umvplus = (avctx->flags & CODEC_FLAG_H263P_UMV) ? 1:0;
	s->h263_aic= (avctx->flags & CODEC_FLAG_H263P_AIC) ? 1:0;
	s->modified_quant= s->h263_aic;
	s->alt_inter_vlc= (avctx->flags & CODEC_FLAG_H263P_AIV) ? 1:0;
	s->obmc= (avctx->flags & CODEC_FLAG_OBMC) ? 1:0;
	s->loop_filter= (avctx->flags & CODEC_FLAG_LOOP_FILTER) ? 1:0;
	s->unrestricted_mv= s->obmc || s->loop_filter || s->umvplus;
        s->h263_slice_structured= (s->flags & CODEC_FLAG_H263P_SLICE_STRUCT) ? 1:0;

	/
        /
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_FLV1:
        s->out_format = FMT_H263;
        s->h263_flv = 2; /
        s->unrestricted_mv = 1;
        s->rtp_mode=0; /
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_RV10:
        s->out_format = FMT_H263;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_RV20:
        s->out_format = FMT_H263;
        avctx->delay=0;
        s->low_delay=1;
        s->modified_quant=1;
        s->h263_aic=1;
        s->h263_plus=1;
        s->loop_filter=1;
        s->unrestricted_mv= s->obmc || s->loop_filter || s->umvplus;
        break;
    case CODEC_ID_MPEG4:
        s->out_format = FMT_H263;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->low_delay= s->max_b_frames ? 0 : 1;
        avctx->delay= s->low_delay ? 0 : (s->max_b_frames + 1);
        break;
    case CODEC_ID_MSMPEG4V1:
        s->out_format = FMT_H263;
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->msmpeg4_version= 1;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_MSMPEG4V2:
        s->out_format = FMT_H263;
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->msmpeg4_version= 2;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_MSMPEG4V3:
        s->out_format = FMT_H263;
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->msmpeg4_version= 3;
        s->flipflop_rounding=1;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_WMV1:
        s->out_format = FMT_H263;
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->msmpeg4_version= 4;
        s->flipflop_rounding=1;
        avctx->delay=0;
        s->low_delay=1;
        break;
    case CODEC_ID_WMV2:
        s->out_format = FMT_H263;
        s->h263_msmpeg4 = 1;
        s->h263_pred = 1;
        s->unrestricted_mv = 1;
        s->msmpeg4_version= 5;
        s->flipflop_rounding=1;
        avctx->delay=0;
        s->low_delay=1;
        break;
#endif
    default:
        return -1;
    }
    
    avctx->has_b_frames= !s->low_delay;

    s->encoding = 1;

    /
    if (MPV_common_init(s) < 0)
        return -1;

    if(s->modified_quant)
        s->chroma_qscale_table= ff_h263_chroma_qscale_table;
    s->progressive_frame= 
    s->progressive_sequence= !(avctx->flags & (CODEC_FLAG_INTERLACED_DCT|CODEC_FLAG_INTERLACED_ME));
    s->quant_precision=5;
    
    ff_set_cmp(&s->dsp, s->dsp.ildct_cmp, s->avctx->ildct_cmp);
    ff_set_cmp(&s->dsp, s->dsp.frame_skip_cmp, s->avctx->frame_skip_cmp);
    
#ifdef CONFIG_ENCODERS
#ifdef CONFIG_RISKY
    if (s->out_format == FMT_H261)
        ff_h261_encode_init(s);
    if (s->out_format == FMT_H263)
        h263_encode_init(s);
    if(s->msmpeg4_version)
        ff_msmpeg4_encode_init(s);
#endif
    if (s->out_format == FMT_MPEG1)
        ff_mpeg1_encode_init(s);
#endif

    /
    for(i=0;i<64;i++) {
        int j= s->dsp.idct_permutation[i];
#ifdef CONFIG_RISKY
        if(s->codec_id==CODEC_ID_MPEG4 && s->mpeg_quant){
            s->intra_matrix[j] = ff_mpeg4_default_intra_matrix[i];
            s->inter_matrix[j] = ff_mpeg4_default_non_intra_matrix[i];
        }else if(s->out_format == FMT_H263 || s->out_format == FMT_H261){
            s->intra_matrix[j] =
            s->inter_matrix[j] = ff_mpeg1_default_non_intra_matrix[i];
        }else
#endif
        { /
            s->intra_matrix[j] = ff_mpeg1_default_intra_matrix[i];
            s->inter_matrix[j] = ff_mpeg1_default_non_intra_matrix[i];
        }
        if(s->avctx->intra_matrix)
            s->intra_matrix[j] = s->avctx->intra_matrix[i];
        if(s->avctx->inter_matrix)
            s->inter_matrix[j] = s->avctx->inter_matrix[i];
    }

    /
    /
    if (s->out_format != FMT_MJPEG) {
        convert_matrix(&s->dsp, s->q_intra_matrix, s->q_intra_matrix16, 
                       s->intra_matrix, s->intra_quant_bias, 1, 31);
        convert_matrix(&s->dsp, s->q_inter_matrix, s->q_inter_matrix16, 
                       s->inter_matrix, s->inter_quant_bias, 1, 31);
    }

    if(ff_rate_control_init(s) < 0)
        return -1;
    
    return 0;
}
