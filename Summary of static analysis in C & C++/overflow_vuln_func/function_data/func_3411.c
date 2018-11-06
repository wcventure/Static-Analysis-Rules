static int poll_filters(void)
{
    AVFilterBufferRef *picref;
    AVFrame *filtered_frame = NULL;
    int i, ret, ret_all;
    unsigned nb_success, nb_eof;
    int64_t frame_pts;

    while (1) {
        /
        for (i = 0; i < nb_output_streams; i++) {
            OutputStream *ost = output_streams[i];
            OutputFile    *of = output_files[ost->file_index];

            if (!ost->filter || ost->is_past_recording_time)
                continue;

            if (!ost->filtered_frame && !(ost->filtered_frame = avcodec_alloc_frame())) {
                return AVERROR(ENOMEM);
            } else
                avcodec_get_frame_defaults(ost->filtered_frame);
            filtered_frame = ost->filtered_frame;

            while (1) {
                AVRational ist_pts_tb = ost->filter->filter->inputs[0]->time_base;
                ret = av_buffersink_get_buffer_ref(ost->filter->filter, &picref,
                                                   AV_BUFFERSINK_FLAG_NO_REQUEST);
                if (ret < 0) {
                    if (ret != AVERROR(EAGAIN)) {
                        char buf[256];
                        av_strerror(ret, buf, sizeof(buf));
                        av_log(NULL, AV_LOG_WARNING,
                               "Error in av_buffersink_get_buffer_ref(): %s\n", buf);
                    }
                    break;
                }
                filtered_frame->pts = frame_pts = av_rescale_q(picref->pts, ist_pts_tb, AV_TIME_BASE_Q);
                //if (ost->source_index >= 0)
                //    *filtered_frame= *input_streams[ost->source_index]->decoded_frame; //for me_threshold

                if (of->start_time && filtered_frame->pts < of->start_time)
                    return 0;

                switch (ost->filter->filter->inputs[0]->type) {
                case AVMEDIA_TYPE_VIDEO:
                    avfilter_fill_frame_from_video_buffer_ref(filtered_frame, picref);
                    filtered_frame->pts = frame_pts;
                    if (!ost->frame_aspect_ratio)
                        ost->st->codec->sample_aspect_ratio = picref->video->sample_aspect_ratio;

                    do_video_out(of->ctx, ost, filtered_frame,
                                 same_quant ? ost->last_quality :
                                              ost->st->codec->global_quality);
                    break;
                default:
                    // TODO support audio/subtitle filters
                    av_assert0(0);
                }

                avfilter_unref_buffer(picref);
            }
        }
        /
        ret_all = nb_success = nb_eof = 0;
        for (i = 0; i < nb_filtergraphs; i++) {
            ret = avfilter_graph_request_oldest(filtergraphs[i]->graph);
            if (!ret) {
                nb_success++;
            } else if (ret == AVERROR_EOF) {
                nb_eof++;
            } else if (ret != AVERROR(EAGAIN)) {
                char buf[256];
                av_strerror(ret, buf, sizeof(buf));
                av_log(NULL, AV_LOG_WARNING,
                       "Error in request_frame(): %s\n", buf);
                ret_all = ret;
            }
        }
        if (!nb_success)
            break;
        /
    }
    return nb_eof == nb_filtergraphs ? AVERROR_EOF : ret_all;
}
