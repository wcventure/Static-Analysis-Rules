static int decode_vop_header(Mpeg4DecContext *ctx, GetBitContext *gb)
{
    MpegEncContext *s = &ctx->m;
    int time_incr, time_increment;
    int64_t pts;

    s->pict_type = get_bits(gb, 2) + AV_PICTURE_TYPE_I;        /
    if (s->pict_type == AV_PICTURE_TYPE_B && s->low_delay &&
        ctx->vol_control_parameters == 0 && !(s->avctx->flags & AV_CODEC_FLAG_LOW_DELAY)) {
        av_log(s->avctx, AV_LOG_ERROR, "low_delay flag set incorrectly, clearing it\n");
        s->low_delay = 0;
    }

    s->partitioned_frame = s->data_partitioning && s->pict_type != AV_PICTURE_TYPE_B;
    if (s->partitioned_frame)
        s->decode_mb = mpeg4_decode_partitioned_mb;
    else
        s->decode_mb = mpeg4_decode_mb;

    time_incr = 0;
    while (get_bits1(gb) != 0)
        time_incr++;

    check_marker(s->avctx, gb, "before time_increment");

    if (ctx->time_increment_bits == 0 ||
        !(show_bits(gb, ctx->time_increment_bits + 1) & 1)) {
        av_log(s->avctx, AV_LOG_WARNING,
               "time_increment_bits %d is invalid in relation to the current bitstream, this is likely caused by a missing VOL header\n", ctx->time_increment_bits);

        for (ctx->time_increment_bits = 1;
             ctx->time_increment_bits < 16;
             ctx->time_increment_bits++) {
            if (s->pict_type == AV_PICTURE_TYPE_P ||
                (s->pict_type == AV_PICTURE_TYPE_S &&
                 ctx->vol_sprite_usage == GMC_SPRITE)) {
                if ((show_bits(gb, ctx->time_increment_bits + 6) & 0x37) == 0x30)
                    break;
            } else if ((show_bits(gb, ctx->time_increment_bits + 5) & 0x1F) == 0x18)
                break;
        }

        av_log(s->avctx, AV_LOG_WARNING,
               "time_increment_bits set to %d bits, based on bitstream analysis\n", ctx->time_increment_bits);
        if (s->avctx->framerate.num && 4*s->avctx->framerate.num < 1<<ctx->time_increment_bits) {
            s->avctx->framerate.num = 1<<ctx->time_increment_bits;
            s->avctx->time_base = av_inv_q(av_mul_q(s->avctx->framerate, (AVRational){s->avctx->ticks_per_frame, 1}));
        }
    }

    if (IS_3IV1)
        time_increment = get_bits1(gb);        // FIXME investigate further
    else
        time_increment = get_bits(gb, ctx->time_increment_bits);

    if (s->pict_type != AV_PICTURE_TYPE_B) {
        s->last_time_base = s->time_base;
        s->time_base     += time_incr;
        s->time = s->time_base * s->avctx->framerate.num + time_increment;
        if (s->workaround_bugs & FF_BUG_UMP4) {
            if (s->time < s->last_non_b_time) {
                /
                s->time_base++;
                s->time += s->avctx->framerate.num;
            }
        }
        s->pp_time         = s->time - s->last_non_b_time;
        s->last_non_b_time = s->time;
    } else {
        s->time    = (s->last_time_base + time_incr) * s->avctx->framerate.num + time_increment;
        s->pb_time = s->pp_time - (s->last_non_b_time - s->time);
        if (s->pp_time <= s->pb_time ||
            s->pp_time <= s->pp_time - s->pb_time ||
            s->pp_time <= 0) {
            /
            return FRAME_SKIPPED;
        }
        ff_mpeg4_init_direct_mv(s);

        if (ctx->t_frame == 0)
            ctx->t_frame = s->pb_time;
        if (ctx->t_frame == 0)
            ctx->t_frame = 1;  // 1/0 protection
        s->pp_field_time = (ROUNDED_DIV(s->last_non_b_time, ctx->t_frame) -
                            ROUNDED_DIV(s->last_non_b_time - s->pp_time, ctx->t_frame)) * 2;
        s->pb_field_time = (ROUNDED_DIV(s->time, ctx->t_frame) -
                            ROUNDED_DIV(s->last_non_b_time - s->pp_time, ctx->t_frame)) * 2;
        if (s->pp_field_time <= s->pb_field_time || s->pb_field_time <= 1) {
            s->pb_field_time = 2;
            s->pp_field_time = 4;
            if (!s->progressive_sequence)
                return FRAME_SKIPPED;
        }
    }

    if (s->avctx->framerate.den)
        pts = ROUNDED_DIV(s->time, s->avctx->framerate.den);
    else
        pts = AV_NOPTS_VALUE;
    ff_dlog(s->avctx, "MPEG4 PTS: %"PRId64"\n", pts);

    check_marker(s->avctx, gb, "before vop_coded");

    /
    if (get_bits1(gb) != 1) {
        if (s->avctx->debug & FF_DEBUG_PICT_INFO)
            av_log(s->avctx, AV_LOG_ERROR, "vop not coded\n");
        return FRAME_SKIPPED;
    }
    if (ctx->new_pred)
        decode_new_pred(ctx, gb);

    if (ctx->shape != BIN_ONLY_SHAPE &&
                    (s->pict_type == AV_PICTURE_TYPE_P ||
                     (s->pict_type == AV_PICTURE_TYPE_S &&
                      ctx->vol_sprite_usage == GMC_SPRITE))) {
        /
        s->no_rounding = get_bits1(gb);
    } else {
        s->no_rounding = 0;
    }
    // FIXME reduced res stuff

    if (ctx->shape != RECT_SHAPE) {
        if (ctx->vol_sprite_usage != 1 || s->pict_type != AV_PICTURE_TYPE_I) {
            skip_bits(gb, 13);  /
            check_marker(s->avctx, gb, "after width");
            skip_bits(gb, 13);  /
            check_marker(s->avctx, gb, "after height");
            skip_bits(gb, 13);  /
            check_marker(s->avctx, gb, "after hor_spat_ref");
            skip_bits(gb, 13);  /
        }
        skip_bits1(gb);         /

        if (get_bits1(gb) != 0)
            skip_bits(gb, 8);   /
    }

    // FIXME complexity estimation stuff

    if (ctx->shape != BIN_ONLY_SHAPE) {
        skip_bits_long(gb, ctx->cplx_estimation_trash_i);
        if (s->pict_type != AV_PICTURE_TYPE_I)
            skip_bits_long(gb, ctx->cplx_estimation_trash_p);
        if (s->pict_type == AV_PICTURE_TYPE_B)
            skip_bits_long(gb, ctx->cplx_estimation_trash_b);

        if (get_bits_left(gb) < 3) {
            av_log(s->avctx, AV_LOG_ERROR, "Header truncated\n");
            return AVERROR_INVALIDDATA;
        }
        ctx->intra_dc_threshold = ff_mpeg4_dc_threshold[get_bits(gb, 3)];
        if (!s->progressive_sequence) {
            s->top_field_first = get_bits1(gb);
            s->alternate_scan  = get_bits1(gb);
        } else
            s->alternate_scan = 0;
    }

    if (s->alternate_scan) {
        ff_init_scantable(s->idsp.idct_permutation, &s->inter_scantable,   ff_alternate_vertical_scan);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_scantable,   ff_alternate_vertical_scan);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_h_scantable, ff_alternate_vertical_scan);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_v_scantable, ff_alternate_vertical_scan);
    } else {
        ff_init_scantable(s->idsp.idct_permutation, &s->inter_scantable,   ff_zigzag_direct);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_scantable,   ff_zigzag_direct);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_h_scantable, ff_alternate_horizontal_scan);
        ff_init_scantable(s->idsp.idct_permutation, &s->intra_v_scantable, ff_alternate_vertical_scan);
    }

    if (s->pict_type == AV_PICTURE_TYPE_S) {
        if((ctx->vol_sprite_usage == STATIC_SPRITE ||
            ctx->vol_sprite_usage == GMC_SPRITE)) {
            if (mpeg4_decode_sprite_trajectory(ctx, gb) < 0)
                return AVERROR_INVALIDDATA;
            if (ctx->sprite_brightness_change)
                av_log(s->avctx, AV_LOG_ERROR,
                    "sprite_brightness_change not supported\n");
            if (ctx->vol_sprite_usage == STATIC_SPRITE)
                av_log(s->avctx, AV_LOG_ERROR, "static sprite not supported\n");
        } else {
            memset(s->sprite_offset, 0, sizeof(s->sprite_offset));
            memset(s->sprite_delta, 0, sizeof(s->sprite_delta));
        }
    }

    if (ctx->shape != BIN_ONLY_SHAPE) {
        s->chroma_qscale = s->qscale = get_bits(gb, s->quant_precision);
        if (s->qscale == 0) {
            av_log(s->avctx, AV_LOG_ERROR,
                   "Error, header damaged or not MPEG-4 header (qscale=0)\n");
            return AVERROR_INVALIDDATA;  // makes no sense to continue, as there is nothing left from the image then
        }

        if (s->pict_type != AV_PICTURE_TYPE_I) {
            s->f_code = get_bits(gb, 3);        /
            if (s->f_code == 0) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "Error, header damaged or not MPEG-4 header (f_code=0)\n");
                s->f_code = 1;
                return AVERROR_INVALIDDATA;  // makes no sense to continue, as there is nothing left from the image then
            }
        } else
            s->f_code = 1;

        if (s->pict_type == AV_PICTURE_TYPE_B) {
            s->b_code = get_bits(gb, 3);
            if (s->b_code == 0) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "Error, header damaged or not MPEG4 header (b_code=0)\n");
                s->b_code=1;
                return AVERROR_INVALIDDATA; // makes no sense to continue, as the MV decoding will break very quickly
            }
        } else
            s->b_code = 1;

        if (s->avctx->debug & FF_DEBUG_PICT_INFO) {
            av_log(s->avctx, AV_LOG_DEBUG,
                   "qp:%d fc:%d,%d %s size:%d pro:%d alt:%d top:%d %spel part:%d resync:%d w:%d a:%d rnd:%d vot:%d%s dc:%d ce:%d/%d/%d time:%"PRId64" tincr:%d\n",
                   s->qscale, s->f_code, s->b_code,
                   s->pict_type == AV_PICTURE_TYPE_I ? "I" : (s->pict_type == AV_PICTURE_TYPE_P ? "P" : (s->pict_type == AV_PICTURE_TYPE_B ? "B" : "S")),
                   gb->size_in_bits,s->progressive_sequence, s->alternate_scan,
                   s->top_field_first, s->quarter_sample ? "q" : "h",
                   s->data_partitioning, ctx->resync_marker,
                   ctx->num_sprite_warping_points, s->sprite_warping_accuracy,
                   1 - s->no_rounding, s->vo_type,
                   ctx->vol_control_parameters ? " VOLC" : " ", ctx->intra_dc_threshold,
                   ctx->cplx_estimation_trash_i, ctx->cplx_estimation_trash_p,
                   ctx->cplx_estimation_trash_b,
                   s->time,
                   time_increment
                  );
        }

        if (!ctx->scalability) {
            if (ctx->shape != RECT_SHAPE && s->pict_type != AV_PICTURE_TYPE_I)
                skip_bits1(gb);  // vop shape coding type
        } else {
            if (ctx->enhancement_type) {
                int load_backward_shape = get_bits1(gb);
                if (load_backward_shape)
                    av_log(s->avctx, AV_LOG_ERROR,
                           "load backward shape isn't supported\n");
            }
            skip_bits(gb, 2);  // ref_select_code
        }
    }
    /
    if (s->vo_type == 0 && ctx->vol_control_parameters == 0 &&
        ctx->divx_version == -1 && s->picture_number == 0) {
        av_log(s->avctx, AV_LOG_WARNING,
               "looks like this file was encoded with (divx4/(old)xvid/opendivx) -> forcing low_delay flag\n");
        s->low_delay = 1;
    }

    s->picture_number++;  // better than pic number==0 always ;)

    // FIXME add short header support
    s->y_dc_scale_table = ff_mpeg4_y_dc_scale_table;
    s->c_dc_scale_table = ff_mpeg4_c_dc_scale_table;

    if (s->workaround_bugs & FF_BUG_EDGE) {
        s->h_edge_pos = s->width;
        s->v_edge_pos = s->height;
    }
    return 0;
}
