void exit_program(int ret)
{
    int i, j;

    for (i = 0; i < nb_filtergraphs; i++) {
        avfilter_graph_free(&filtergraphs[i]->graph);
        for (j = 0; j < filtergraphs[i]->nb_inputs; j++)
            av_freep(&filtergraphs[i]->inputs[j]);
        av_freep(&filtergraphs[i]->inputs);
        for (j = 0; j < filtergraphs[i]->nb_outputs; j++)
            av_freep(&filtergraphs[i]->outputs[j]);
        av_freep(&filtergraphs[i]->outputs);
        av_freep(&filtergraphs[i]);
    }
    av_freep(&filtergraphs);

    /
    for (i = 0; i < nb_output_files; i++) {
        AVFormatContext *s = output_files[i]->ctx;
        if (!(s->oformat->flags & AVFMT_NOFILE) && s->pb)
            avio_close(s->pb);
        avformat_free_context(s);
        av_dict_free(&output_files[i]->opts);
        av_freep(&output_files[i]);
    }
    for (i = 0; i < nb_output_streams; i++) {
        AVBitStreamFilterContext *bsfc = output_streams[i]->bitstream_filters;
        while (bsfc) {
            AVBitStreamFilterContext *next = bsfc->next;
            av_bitstream_filter_close(bsfc);
            bsfc = next;
        }
        output_streams[i]->bitstream_filters = NULL;

        if (output_streams[i]->output_frame) {
            AVFrame *frame = output_streams[i]->output_frame;
            if (frame->extended_data != frame->data)
                av_freep(&frame->extended_data);
            av_freep(&frame);
        }

        av_freep(&output_streams[i]->avfilter);
        av_freep(&output_streams[i]->filtered_frame);
        av_freep(&output_streams[i]);
    }
    for (i = 0; i < nb_input_files; i++) {
        avformat_close_input(&input_files[i]->ctx);
        av_freep(&input_files[i]);
    }
    for (i = 0; i < nb_input_streams; i++) {
        av_freep(&input_streams[i]->decoded_frame);
        av_dict_free(&input_streams[i]->opts);
        free_buffer_pool(input_streams[i]);
        av_freep(&input_streams[i]->filters);
        av_freep(&input_streams[i]);
    }

    if (vstats_file)
        fclose(vstats_file);
    av_free(vstats_filename);

    av_freep(&input_streams);
    av_freep(&input_files);
    av_freep(&output_streams);
    av_freep(&output_files);

    uninit_opts();
    av_free(audio_buf);
    allocated_audio_buf_size = 0;
    av_free(async_buf);
    allocated_async_buf_size = 0;

    avfilter_uninit();
    avformat_network_deinit();

    if (received_sigterm) {
        av_log(NULL, AV_LOG_INFO, "Received signal %d: terminating.\n",
               (int) received_sigterm);
        exit (255);
    }

    exit(ret);
}
