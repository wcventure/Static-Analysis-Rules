static int vp8_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                            AVPacket *avpkt)
{
    VP8Context *s = avctx->priv_data;
    int ret, mb_x, mb_y, i, y, referenced;
    enum AVDiscard skip_thresh;
    AVFrame *av_uninit(curframe), *prev_frame = s->framep[VP56_FRAME_CURRENT];

    release_queued_segmaps(s, 0);

    if ((ret = decode_frame_header(s, avpkt->data, avpkt->size)) < 0)
        return ret;

    referenced = s->update_last || s->update_golden == VP56_FRAME_CURRENT
                                || s->update_altref == VP56_FRAME_CURRENT;

    skip_thresh = !referenced ? AVDISCARD_NONREF :
                    !s->keyframe ? AVDISCARD_NONKEY : AVDISCARD_ALL;

    if (avctx->skip_frame >= skip_thresh) {
        s->invisible = 1;
        goto skip_decode;
    }
    s->deblock_filter = s->filter.level && avctx->skip_loop_filter < skip_thresh;

    // release no longer referenced frames
    for (i = 0; i < 5; i++)
        if (s->frames[i].data[0] &&
            &s->frames[i] != prev_frame &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2])
            vp8_release_frame(s, &s->frames[i], 0);

    // find a free buffer
    for (i = 0; i < 5; i++)
        if (&s->frames[i] != prev_frame &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2]) {
            curframe = s->framep[VP56_FRAME_CURRENT] = &s->frames[i];
            break;
        }
    if (i == 5) {
        av_log(avctx, AV_LOG_FATAL, "Ran out of free frames!\n");
        abort();
    }
    if (curframe->data[0])
        ff_thread_release_buffer(avctx, curframe);

    curframe->key_frame = s->keyframe;
    curframe->pict_type = s->keyframe ? AV_PICTURE_TYPE_I : AV_PICTURE_TYPE_P;
    curframe->reference = referenced ? 3 : 0;
    if ((ret = vp8_alloc_frame(s, curframe))) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed!\n");
        return ret;
    }

    // check if golden and altref are swapped
    if (s->update_altref != VP56_FRAME_NONE) {
        s->next_framep[VP56_FRAME_GOLDEN2]  = s->framep[s->update_altref];
    } else {
        s->next_framep[VP56_FRAME_GOLDEN2]  = s->framep[VP56_FRAME_GOLDEN2];
    }
    if (s->update_golden != VP56_FRAME_NONE) {
        s->next_framep[VP56_FRAME_GOLDEN]   = s->framep[s->update_golden];
    } else {
        s->next_framep[VP56_FRAME_GOLDEN]   = s->framep[VP56_FRAME_GOLDEN];
    }
    if (s->update_last) {
        s->next_framep[VP56_FRAME_PREVIOUS] = curframe;
    } else {
        s->next_framep[VP56_FRAME_PREVIOUS] = s->framep[VP56_FRAME_PREVIOUS];
    }
    s->next_framep[VP56_FRAME_CURRENT]      = curframe;

    ff_thread_finish_setup(avctx);

    // Given that arithmetic probabilities are updated every frame, it's quite likely
    // that the values we have on a random interframe are complete junk if we didn't
    // start decode on a keyframe. So just don't display anything rather than junk.
    if (!s->keyframe && (!s->framep[VP56_FRAME_PREVIOUS] ||
                         !s->framep[VP56_FRAME_GOLDEN] ||
                         !s->framep[VP56_FRAME_GOLDEN2])) {
        av_log(avctx, AV_LOG_WARNING, "Discarding interframe without a prior keyframe!\n");
        return AVERROR_INVALIDDATA;
    }

    s->linesize   = curframe->linesize[0];
    s->uvlinesize = curframe->linesize[1];

    if (!s->edge_emu_buffer)
        s->edge_emu_buffer = av_malloc(21*s->linesize);

    memset(s->top_nnz, 0, s->mb_width*sizeof(*s->top_nnz));

    /
    memset(s->macroblocks + s->mb_height*2 - 1, 0, (s->mb_width+1)*sizeof(*s->macroblocks));

    // top edge of 127 for intra prediction
    if (!(avctx->flags & CODEC_FLAG_EMU_EDGE)) {
        s->top_border[0][15] = s->top_border[0][23] = 127;
        memset(s->top_border[1]-1, 127, s->mb_width*sizeof(*s->top_border)+1);
    }
    memset(s->ref_count, 0, sizeof(s->ref_count));
    if (s->keyframe)
        memset(s->intra4x4_pred_mode_top, DC_PRED, s->mb_width*4);

#define MARGIN (16 << 2)
    s->mv_min.y = -MARGIN;
    s->mv_max.y = ((s->mb_height - 1) << 6) + MARGIN;

    for (mb_y = 0; mb_y < s->mb_height; mb_y++) {
        VP56RangeCoder *c = &s->coeff_partition[mb_y & (s->num_coeff_partitions-1)];
        VP8Macroblock *mb = s->macroblocks + (s->mb_height - mb_y - 1)*2;
        int mb_xy = mb_y*s->mb_width;
        uint8_t *dst[3] = {
            curframe->data[0] + 16*mb_y*s->linesize,
            curframe->data[1] +  8*mb_y*s->uvlinesize,
            curframe->data[2] +  8*mb_y*s->uvlinesize
        };

        memset(mb - 1, 0, sizeof(*mb));   // zero left macroblock
        memset(s->left_nnz, 0, sizeof(s->left_nnz));
        AV_WN32A(s->intra4x4_pred_mode_left, DC_PRED*0x01010101);

        // left edge of 129 for intra prediction
        if (!(avctx->flags & CODEC_FLAG_EMU_EDGE)) {
            for (i = 0; i < 3; i++)
                for (y = 0; y < 16>>!!i; y++)
                    dst[i][y*curframe->linesize[i]-1] = 129;
            if (mb_y == 1) // top left edge is also 129
                s->top_border[0][15] = s->top_border[0][23] = s->top_border[0][31] = 129;
        }

        s->mv_min.x = -MARGIN;
        s->mv_max.x = ((s->mb_width  - 1) << 6) + MARGIN;
        if (prev_frame && s->segmentation.enabled && !s->segmentation.update_map)
            ff_thread_await_progress(prev_frame, mb_y, 0);

        for (mb_x = 0; mb_x < s->mb_width; mb_x++, mb_xy++, mb++) {
            /
            s->dsp.prefetch(dst[0] + (mb_x&3)*4*s->linesize + 64, s->linesize, 4);
            s->dsp.prefetch(dst[1] + (mb_x&7)*s->uvlinesize + 64, dst[2] - dst[1], 2);

            decode_mb_mode(s, mb, mb_x, mb_y, curframe->ref_index[0] + mb_xy,
                           prev_frame && prev_frame->ref_index[0] ? prev_frame->ref_index[0] + mb_xy : NULL);

            prefetch_motion(s, mb, mb_x, mb_y, mb_xy, VP56_FRAME_PREVIOUS);

            if (!mb->skip)
                decode_mb_coeffs(s, c, mb, s->top_nnz[mb_x], s->left_nnz);

            if (mb->mode <= MODE_I4x4)
                intra_predict(s, dst, mb, mb_x, mb_y);
            else
                inter_predict(s, dst, mb, mb_x, mb_y);

            prefetch_motion(s, mb, mb_x, mb_y, mb_xy, VP56_FRAME_GOLDEN);

            if (!mb->skip) {
                idct_mb(s, dst, mb);
            } else {
                AV_ZERO64(s->left_nnz);
                AV_WN64(s->top_nnz[mb_x], 0);   // array of 9, so unaligned

                // Reset DC block predictors if they would exist if the mb had coefficients
                if (mb->mode != MODE_I4x4 && mb->mode != VP8_MVMODE_SPLIT) {
                    s->left_nnz[8]      = 0;
                    s->top_nnz[mb_x][8] = 0;
                }
            }

            if (s->deblock_filter)
                filter_level_for_mb(s, mb, &s->filter_strength[mb_x]);

            prefetch_motion(s, mb, mb_x, mb_y, mb_xy, VP56_FRAME_GOLDEN2);

            dst[0] += 16;
            dst[1] += 8;
            dst[2] += 8;
            s->mv_min.x -= 64;
            s->mv_max.x -= 64;
        }
        if (s->deblock_filter) {
            if (s->filter.simple)
                filter_mb_row_simple(s, curframe, mb_y);
            else
                filter_mb_row(s, curframe, mb_y);
        }
        s->mv_min.y -= 64;
        s->mv_max.y -= 64;

        ff_thread_report_progress(curframe, mb_y, 0);
    }

    ff_thread_report_progress(curframe, INT_MAX, 0);
skip_decode:
    // if future frames don't use the updated probabilities,
    // reset them to the values we saved
    if (!s->update_probabilities)
        s->prob[0] = s->prob[1];

    memcpy(&s->framep[0], &s->next_framep[0], sizeof(s->framep[0]) * 4);

    if (!s->invisible) {
        *(AVFrame*)data = *curframe;
        *data_size = sizeof(AVFrame);
    }

    return avpkt->size;
}
