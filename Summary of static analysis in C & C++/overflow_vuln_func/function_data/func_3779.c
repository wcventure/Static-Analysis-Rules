static int poll_filters(void)
{
    AVFilterBufferRef *picref;
    AVFrame *filtered_frame = NULL;
    int i, frame_size;

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

        while (av_buffersink_read(ost->filter->filter, &picref) >= 0) {
            avfilter_copy_buf_props(filtered_frame, picref);
            filtered_frame->pts = av_rescale_q(picref->pts,
                                               ost->filter->filter->inputs[0]->time_base,
                                               AV_TIME_BASE_Q);

            if (of->start_time && filtered_frame->pts < of->start_time)
                return 0;

            switch (ost->filter->filter->inputs[0]->type) {
            case AVMEDIA_TYPE_VIDEO:
                if (!ost->frame_aspect_ratio)
                    ost->st->codec->sample_aspect_ratio = picref->video->pixel_aspect;

                do_video_out(of->ctx, ost, filtered_frame, &frame_size,
                             same_quant ? ost->last_quality :
                                          ost->st->codec->global_quality);
                if (vstats_filename && frame_size)
                    do_video_stats(of->ctx, ost, frame_size);
                break;
            default:
                // TODO support audio/subtitle filters
                av_assert0(0);
            }

            avfilter_unref_buffer(picref);
        }
    }
    return 0;
}
