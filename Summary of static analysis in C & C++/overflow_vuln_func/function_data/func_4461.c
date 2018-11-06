static int decode_slice(struct AVCodecContext *avctx, void *arg)
{
    H264SliceContext *sl = arg;
    const H264Context *h = sl->h264;
    int lf_x_start = sl->mb_x;
    int ret;

    sl->linesize   = h->cur_pic_ptr->f->linesize[0];
    sl->uvlinesize = h->cur_pic_ptr->f->linesize[1];

    ret = alloc_scratch_buffers(sl, sl->linesize);
    if (ret < 0)
        return ret;

    sl->mb_skip_run = -1;

    av_assert0(h->block_offset[15] == (4 * ((scan8[15] - scan8[0]) & 7) << h->pixel_shift) + 4 * sl->linesize * ((scan8[15] - scan8[0]) >> 3));

    sl->is_complex = FRAME_MBAFF(h) || h->picture_structure != PICT_FRAME ||
                     avctx->codec_id != AV_CODEC_ID_H264 ||
                     (CONFIG_GRAY && (h->flags & AV_CODEC_FLAG_GRAY));

    if (!(h->avctx->active_thread_type & FF_THREAD_SLICE) && h->picture_structure == PICT_FRAME && h->slice_ctx[0].er.error_status_table) {
        const int start_i  = av_clip(sl->resync_mb_x + sl->resync_mb_y * h->mb_width, 0, h->mb_num - 1);
        if (start_i) {
            int prev_status = h->slice_ctx[0].er.error_status_table[h->slice_ctx[0].er.mb_index2xy[start_i - 1]];
            prev_status &= ~ VP_START;
            if (prev_status != (ER_MV_END | ER_DC_END | ER_AC_END))
                h->slice_ctx[0].er.error_occurred = 1;
        }
    }

    if (h->pps.cabac) {
        /
        align_get_bits(&sl->gb);

        /
        ff_init_cabac_decoder(&sl->cabac,
                              sl->gb.buffer + get_bits_count(&sl->gb) / 8,
                              (get_bits_left(&sl->gb) + 7) / 8);

        ff_h264_init_cabac_states(h, sl);

        for (;;) {
            // START_TIMER
            int ret, eos;
            if (sl->mb_x + sl->mb_y * h->mb_width >= sl->next_slice_idx) {
                av_log(h->avctx, AV_LOG_ERROR, "Slice overlaps with next at %d\n",
                       sl->next_slice_idx);
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x,
                             sl->mb_y, ER_MB_ERROR);
                return AVERROR_INVALIDDATA;
            }

            ret = ff_h264_decode_mb_cabac(h, sl);
            // STOP_TIMER("decode_mb_cabac")

            if (ret >= 0)
                ff_h264_hl_decode_mb(h, sl);

            // FIXME optimal? or let mb_decode decode 16x32 ?
            if (ret >= 0 && FRAME_MBAFF(h)) {
                sl->mb_y++;

                ret = ff_h264_decode_mb_cabac(h, sl);

                if (ret >= 0)
                    ff_h264_hl_decode_mb(h, sl);
                sl->mb_y--;
            }
            eos = get_cabac_terminate(&sl->cabac);

            if ((h->workaround_bugs & FF_BUG_TRUNCATED) &&
                sl->cabac.bytestream > sl->cabac.bytestream_end + 2) {
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x - 1,
                             sl->mb_y, ER_MB_END);
                if (sl->mb_x >= lf_x_start)
                    loop_filter(h, sl, lf_x_start, sl->mb_x + 1);
                return 0;
            }
            if (sl->cabac.bytestream > sl->cabac.bytestream_end + 2 )
                av_log(h->avctx, AV_LOG_DEBUG, "bytestream overread %"PTRDIFF_SPECIFIER"\n", sl->cabac.bytestream_end - sl->cabac.bytestream);
            if (ret < 0 || sl->cabac.bytestream > sl->cabac.bytestream_end + 4) {
                av_log(h->avctx, AV_LOG_ERROR,
                       "error while decoding MB %d %d, bytestream %"PTRDIFF_SPECIFIER"\n",
                       sl->mb_x, sl->mb_y,
                       sl->cabac.bytestream_end - sl->cabac.bytestream);
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x,
                             sl->mb_y, ER_MB_ERROR);
                return AVERROR_INVALIDDATA;
            }

            if (++sl->mb_x >= h->mb_width) {
                loop_filter(h, sl, lf_x_start, sl->mb_x);
                sl->mb_x = lf_x_start = 0;
                decode_finish_row(h, sl);
                ++sl->mb_y;
                if (FIELD_OR_MBAFF_PICTURE(h)) {
                    ++sl->mb_y;
                    if (FRAME_MBAFF(h) && sl->mb_y < h->mb_height)
                        predict_field_decoding_flag(h, sl);
                }
            }

            if (eos || sl->mb_y >= h->mb_height) {
                ff_tlog(h->avctx, "slice end %d %d\n",
                        get_bits_count(&sl->gb), sl->gb.size_in_bits);
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x - 1,
                             sl->mb_y, ER_MB_END);
                if (sl->mb_x > lf_x_start)
                    loop_filter(h, sl, lf_x_start, sl->mb_x);
                return 0;
            }
        }
    } else {
        for (;;) {
            int ret;

            if (sl->mb_x + sl->mb_y * h->mb_width >= sl->next_slice_idx) {
                av_log(h->avctx, AV_LOG_ERROR, "Slice overlaps with next at %d\n",
                       sl->next_slice_idx);
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x,
                             sl->mb_y, ER_MB_ERROR);
                return AVERROR_INVALIDDATA;
            }

            ret = ff_h264_decode_mb_cavlc(h, sl);

            if (ret >= 0)
                ff_h264_hl_decode_mb(h, sl);

            // FIXME optimal? or let mb_decode decode 16x32 ?
            if (ret >= 0 && FRAME_MBAFF(h)) {
                sl->mb_y++;
                ret = ff_h264_decode_mb_cavlc(h, sl);

                if (ret >= 0)
                    ff_h264_hl_decode_mb(h, sl);
                sl->mb_y--;
            }

            if (ret < 0) {
                av_log(h->avctx, AV_LOG_ERROR,
                       "error while decoding MB %d %d\n", sl->mb_x, sl->mb_y);
                er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x,
                             sl->mb_y, ER_MB_ERROR);
                return ret;
            }

            if (++sl->mb_x >= h->mb_width) {
                loop_filter(h, sl, lf_x_start, sl->mb_x);
                sl->mb_x = lf_x_start = 0;
                decode_finish_row(h, sl);
                ++sl->mb_y;
                if (FIELD_OR_MBAFF_PICTURE(h)) {
                    ++sl->mb_y;
                    if (FRAME_MBAFF(h) && sl->mb_y < h->mb_height)
                        predict_field_decoding_flag(h, sl);
                }
                if (sl->mb_y >= h->mb_height) {
                    ff_tlog(h->avctx, "slice end %d %d\n",
                            get_bits_count(&sl->gb), sl->gb.size_in_bits);

                    if (   get_bits_left(&sl->gb) == 0
                        || get_bits_left(&sl->gb) > 0 && !(h->avctx->err_recognition & AV_EF_AGGRESSIVE)) {
                        er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y,
                                     sl->mb_x - 1, sl->mb_y, ER_MB_END);

                        return 0;
                    } else {
                        er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y,
                                     sl->mb_x, sl->mb_y, ER_MB_END);

                        return AVERROR_INVALIDDATA;
                    }
                }
            }

            if (get_bits_left(&sl->gb) <= 0 && sl->mb_skip_run <= 0) {
                ff_tlog(h->avctx, "slice end %d %d\n",
                        get_bits_count(&sl->gb), sl->gb.size_in_bits);

                if (get_bits_left(&sl->gb) == 0) {
                    er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y,
                                 sl->mb_x - 1, sl->mb_y, ER_MB_END);
                    if (sl->mb_x > lf_x_start)
                        loop_filter(h, sl, lf_x_start, sl->mb_x);

                    return 0;
                } else {
                    er_add_slice(sl, sl->resync_mb_x, sl->resync_mb_y, sl->mb_x,
                                 sl->mb_y, ER_MB_ERROR);

                    return AVERROR_INVALIDDATA;
                }
            }
        }
    }
}
