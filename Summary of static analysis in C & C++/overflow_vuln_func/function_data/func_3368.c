static int decode_video(InputStream *ist, AVPacket *pkt, int *got_output)
{
    AVFrame *decoded_frame, *f;
    int i, ret = 0, err = 0, resample_changed;
    int64_t best_effort_timestamp;
    AVRational *frame_sample_aspect;

    if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    decoded_frame = ist->decoded_frame;
    pkt->dts  = av_rescale_q(ist->dts, AV_TIME_BASE_Q, ist->st->time_base);

    update_benchmark(NULL);
    ret = avcodec_decode_video2(ist->dec_ctx,
                                decoded_frame, got_output, pkt);
    update_benchmark("decode_video %d.%d", ist->file_index, ist->st->index);

    // The following line may be required in some cases where there is no parser
    // or the parser does not has_b_frames correctly
    if (ist->st->codec->has_b_frames < ist->dec_ctx->has_b_frames) {
        if (ist->dec_ctx->codec_id == AV_CODEC_ID_H264) {
            ist->st->codec->has_b_frames = ist->dec_ctx->has_b_frames;
        } else
            av_log(ist->dec_ctx, AV_LOG_WARNING,
                   "has_b_frames is larger in decoder than demuxer %d > %d.\n"
                   "If you want to help, upload a sample "
                   "of this file to ftp://upload.ffmpeg.org/incoming/ "
                   "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)",
                   ist->dec_ctx->has_b_frames,
                   ist->st->codec->has_b_frames);
    }

    check_decode_result(ist, got_output, ret);

    if (*got_output && ret >= 0) {
        if (ist->dec_ctx->width  != decoded_frame->width ||
            ist->dec_ctx->height != decoded_frame->height ||
            ist->dec_ctx->pix_fmt != decoded_frame->format) {
            av_log(NULL, AV_LOG_DEBUG, "Frame parameters mismatch context %d,%d,%d != %d,%d,%d\n",
                decoded_frame->width,
                decoded_frame->height,
                decoded_frame->format,
                ist->dec_ctx->width,
                ist->dec_ctx->height,
                ist->dec_ctx->pix_fmt);
        }
    }

    if (!*got_output || ret < 0)
        return ret;

    if(ist->top_field_first>=0)
        decoded_frame->top_field_first = ist->top_field_first;

    ist->frames_decoded++;

    if (ist->hwaccel_retrieve_data && decoded_frame->format == ist->hwaccel_pix_fmt) {
        err = ist->hwaccel_retrieve_data(ist->dec_ctx, decoded_frame);
        if (err < 0)
            goto fail;
    }
    ist->hwaccel_retrieved_pix_fmt = decoded_frame->format;

    best_effort_timestamp= av_frame_get_best_effort_timestamp(decoded_frame);
    if(best_effort_timestamp != AV_NOPTS_VALUE)
        ist->next_pts = ist->pts = av_rescale_q(decoded_frame->pts = best_effort_timestamp, ist->st->time_base, AV_TIME_BASE_Q);

    if (debug_ts) {
        av_log(NULL, AV_LOG_INFO, "decoder -> ist_index:%d type:video "
               "frame_pts:%s frame_pts_time:%s best_effort_ts:%"PRId64" best_effort_ts_time:%s keyframe:%d frame_type:%d time_base:%d/%d\n",
               ist->st->index, av_ts2str(decoded_frame->pts),
               av_ts2timestr(decoded_frame->pts, &ist->st->time_base),
               best_effort_timestamp,
               av_ts2timestr(best_effort_timestamp, &ist->st->time_base),
               decoded_frame->key_frame, decoded_frame->pict_type,
               ist->st->time_base.num, ist->st->time_base.den);
    }

    pkt->size = 0;

    if (ist->st->sample_aspect_ratio.num)
        decoded_frame->sample_aspect_ratio = ist->st->sample_aspect_ratio;

    resample_changed = ist->resample_width   != decoded_frame->width  ||
                       ist->resample_height  != decoded_frame->height ||
                       ist->resample_pix_fmt != decoded_frame->format;
    if (resample_changed) {
        av_log(NULL, AV_LOG_INFO,
               "Input stream #%d:%d frame changed from size:%dx%d fmt:%s to size:%dx%d fmt:%s\n",
               ist->file_index, ist->st->index,
               ist->resample_width,  ist->resample_height,  av_get_pix_fmt_name(ist->resample_pix_fmt),
               decoded_frame->width, decoded_frame->height, av_get_pix_fmt_name(decoded_frame->format));

        ist->resample_width   = decoded_frame->width;
        ist->resample_height  = decoded_frame->height;
        ist->resample_pix_fmt = decoded_frame->format;

        for (i = 0; i < nb_filtergraphs; i++) {
            if (ist_in_filtergraph(filtergraphs[i], ist) && ist->reinit_filters &&
                configure_filtergraph(filtergraphs[i]) < 0) {
                av_log(NULL, AV_LOG_FATAL, "Error reinitializing filters!\n");
                exit_program(1);
            }
        }
    }

    frame_sample_aspect= av_opt_ptr(avcodec_get_frame_class(), decoded_frame, "sample_aspect_ratio");
    for (i = 0; i < ist->nb_filters; i++) {
        if (!frame_sample_aspect->num)
            *frame_sample_aspect = ist->st->sample_aspect_ratio;

        if (i < ist->nb_filters - 1) {
            f = ist->filter_frame;
            err = av_frame_ref(f, decoded_frame);
            if (err < 0)
                break;
        } else
            f = decoded_frame;
        ret = av_buffersrc_add_frame_flags(ist->filters[i]->filter, f, AV_BUFFERSRC_FLAG_PUSH);
        if (ret == AVERROR_EOF) {
            ret = 0; /
        } else if (ret < 0) {
            av_log(NULL, AV_LOG_FATAL,
                   "Failed to inject frame into filter network: %s\n", av_err2str(ret));
            exit_program(1);
        }
    }

fail:
    av_frame_unref(ist->filter_frame);
    av_frame_unref(decoded_frame);
    return err < 0 ? err : ret;
}
