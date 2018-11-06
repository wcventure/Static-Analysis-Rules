static int xvid_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                             const AVFrame *picture, int *got_packet)
{
    int xerr, i, ret, user_packet = !!pkt->data;
    struct xvid_context *x = avctx->priv_data;
    AVFrame *p             = avctx->coded_frame;
    int mb_width  = (avctx->width  + 15) / 16;
    int mb_height = (avctx->height + 15) / 16;
    char *tmp;

    xvid_enc_frame_t xvid_enc_frame = { 0 };
    xvid_enc_stats_t xvid_enc_stats = { 0 };

    if ((ret = ff_alloc_packet2(avctx, pkt, mb_width*mb_height*MAX_MB_BYTES + FF_MIN_BUFFER_SIZE)) < 0)
        return ret;

    /
    xvid_enc_frame.version = XVID_VERSION;
    xvid_enc_stats.version = XVID_VERSION;

    /
    xvid_enc_frame.bitstream = pkt->data;
    xvid_enc_frame.length    = pkt->size;

    /
    if (avctx->pix_fmt != AV_PIX_FMT_YUV420P) {
        av_log(avctx, AV_LOG_ERROR,
               "Xvid: Color spaces other than 420P not supported\n");
        return AVERROR(EINVAL);
    }

    xvid_enc_frame.input.csp = XVID_CSP_PLANAR; /

    for (i = 0; i < 4; i++) {
        xvid_enc_frame.input.plane[i]  = picture->data[i];
        xvid_enc_frame.input.stride[i] = picture->linesize[i];
    }

    /
    xvid_enc_frame.vop_flags = x->vop_flags;
    xvid_enc_frame.vol_flags = x->vol_flags;
    xvid_enc_frame.motion    = x->me_flags;
    xvid_enc_frame.type      =
        picture->pict_type == AV_PICTURE_TYPE_I ? XVID_TYPE_IVOP :
        picture->pict_type == AV_PICTURE_TYPE_P ? XVID_TYPE_PVOP :
        picture->pict_type == AV_PICTURE_TYPE_B ? XVID_TYPE_BVOP :
                                                  XVID_TYPE_AUTO;

    /
    if (avctx->sample_aspect_ratio.num < 0 || avctx->sample_aspect_ratio.num > 255 ||
        avctx->sample_aspect_ratio.den < 0 || avctx->sample_aspect_ratio.den > 255) {
        av_log(avctx, AV_LOG_WARNING,
               "Invalid pixel aspect ratio %i/%i, limit is 255/255 reducing\n",
               avctx->sample_aspect_ratio.num, avctx->sample_aspect_ratio.den);
        av_reduce(&avctx->sample_aspect_ratio.num, &avctx->sample_aspect_ratio.den,
                   avctx->sample_aspect_ratio.num,  avctx->sample_aspect_ratio.den, 255);
    }
    xvid_enc_frame.par        = XVID_PAR_EXT;
    xvid_enc_frame.par_width  = avctx->sample_aspect_ratio.num;
    xvid_enc_frame.par_height = avctx->sample_aspect_ratio.den;

    /
    if (x->qscale)
        xvid_enc_frame.quant = picture->quality / FF_QP2LAMBDA;
    else
        xvid_enc_frame.quant = 0;

    /
    xvid_enc_frame.quant_intra_matrix = x->intra_matrix;
    xvid_enc_frame.quant_inter_matrix = x->inter_matrix;

    /
    xerr = xvid_encore(x->encoder_handle, XVID_ENC_ENCODE,
                       &xvid_enc_frame, &xvid_enc_stats);

    /
    avctx->stats_out = NULL;
    if (x->twopassbuffer) {
        tmp                  = x->old_twopassbuffer;
        x->old_twopassbuffer = x->twopassbuffer;
        x->twopassbuffer     = tmp;
        x->twopassbuffer[0]  = 0;
        if (x->old_twopassbuffer[0] != 0) {
            avctx->stats_out = x->old_twopassbuffer;
        }
    }

    if (xerr > 0) {
        *got_packet = 1;

        p->quality = xvid_enc_stats.quant * FF_QP2LAMBDA;
        if (xvid_enc_stats.type == XVID_TYPE_PVOP)
            p->pict_type = AV_PICTURE_TYPE_P;
        else if (xvid_enc_stats.type == XVID_TYPE_BVOP)
            p->pict_type = AV_PICTURE_TYPE_B;
        else if (xvid_enc_stats.type == XVID_TYPE_SVOP)
            p->pict_type = AV_PICTURE_TYPE_S;
        else
            p->pict_type = AV_PICTURE_TYPE_I;
        if (xvid_enc_frame.out_flags & XVID_KEYFRAME) {
            p->key_frame = 1;
            pkt->flags  |= AV_PKT_FLAG_KEY;
            if (x->quicktime_format)
                return xvid_strip_vol_header(avctx, pkt,
                                             xvid_enc_stats.hlength, xerr);
        } else
            p->key_frame = 0;

        pkt->size = xerr;

        return 0;
    } else {
        if (!user_packet)
            av_free_packet(pkt);
        if (!xerr)
            return 0;
        av_log(avctx, AV_LOG_ERROR,
               "Xvid: Encoding Error Occurred: %i\n", xerr);
        return AVERROR_EXTERNAL;
    }
}
