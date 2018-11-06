static int transcode(void)
{
    int ret, i;
    AVFormatContext *is, *os;
    OutputStream *ost;
    InputStream *ist;
    uint8_t *no_packet;
    int no_packet_count = 0;
    int64_t timer_start;

    if (!(no_packet = av_mallocz(nb_input_files)))
        exit_program(1);

    ret = transcode_init();
    if (ret < 0)
        goto fail;

    av_log(NULL, AV_LOG_INFO, "Press ctrl-c to stop encoding\n");
    term_init();

    timer_start = av_gettime();

    for (; received_sigterm == 0;) {
        int file_index, ist_index, past_recording_time = 1;
        AVPacket pkt;
        int64_t ipts_min;

        ipts_min = INT64_MAX;

        /
        for (i = 0; i < nb_output_streams; i++) {
            OutputFile *of;
            ost = output_streams[i];
            of  = output_files[ost->file_index];
            os  = output_files[ost->file_index]->ctx;
            if (ost->is_past_recording_time ||
                (os->pb && avio_tell(os->pb) >= of->limit_filesize))
                continue;
            if (ost->frame_number > ost->max_frames) {
                int j;
                for (j = 0; j < of->ctx->nb_streams; j++)
                    output_streams[of->ost_index + j]->is_past_recording_time = 1;
                continue;
            }
            past_recording_time = 0;
        }
        if (past_recording_time)
            break;

        /
        file_index = -1;
        for (i = 0; i < nb_input_streams; i++) {
            int64_t ipts;
            ist = input_streams[i];
            ipts = ist->last_dts;
            if (ist->discard || no_packet[ist->file_index])
                continue;
            if (!input_files[ist->file_index]->eof_reached) {
                if (ipts < ipts_min) {
                    ipts_min = ipts;
                    file_index = ist->file_index;
                }
            }
        }
        /
        if (file_index < 0) {
            if (no_packet_count) {
                no_packet_count = 0;
                memset(no_packet, 0, nb_input_files);
                usleep(10000);
                continue;
            }
            break;
        }

        /
        is  = input_files[file_index]->ctx;
        ret = av_read_frame(is, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            no_packet[file_index] = 1;
            no_packet_count++;
            continue;
        }
        if (ret < 0) {
            input_files[file_index]->eof_reached = 1;

            for (i = 0; i < input_files[file_index]->nb_streams; i++) {
                ist = input_streams[input_files[file_index]->ist_index + i];
                if (ist->decoding_needed)
                    output_packet(ist, NULL);
            }

            if (opt_shortest)
                break;
            else
                continue;
        }

        no_packet_count = 0;
        memset(no_packet, 0, nb_input_files);

        if (do_pkt_dump) {
            av_pkt_dump_log2(NULL, AV_LOG_DEBUG, &pkt, do_hex_dump,
                             is->streams[pkt.stream_index]);
        }
        /
        if (pkt.stream_index >= input_files[file_index]->nb_streams)
            goto discard_packet;
        ist_index = input_files[file_index]->ist_index + pkt.stream_index;
        ist = input_streams[ist_index];
        if (ist->discard)
            goto discard_packet;

        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts += av_rescale_q(input_files[ist->file_index]->ts_offset, AV_TIME_BASE_Q, ist->st->time_base);
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts += av_rescale_q(input_files[ist->file_index]->ts_offset, AV_TIME_BASE_Q, ist->st->time_base);

        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts *= ist->ts_scale;
        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts *= ist->ts_scale;

        //fprintf(stderr, "next:%"PRId64" dts:%"PRId64" off:%"PRId64" %d\n",
        //        ist->next_dts,
        //        pkt.dts, input_files[ist->file_index].ts_offset,
        //        ist->st->codec->codec_type);
        if (pkt.dts != AV_NOPTS_VALUE && ist->next_dts != AV_NOPTS_VALUE
            && (is->iformat->flags & AVFMT_TS_DISCONT)) {
            int64_t pkt_dts = av_rescale_q(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q);
            int64_t delta   = pkt_dts - ist->next_dts;
            if ((FFABS(delta) > 1LL * dts_delta_threshold * AV_TIME_BASE || pkt_dts + 1 < ist->last_dts) && !copy_ts) {
                input_files[ist->file_index]->ts_offset -= delta;
                av_log(NULL, AV_LOG_DEBUG,
                       "timestamp discontinuity %"PRId64", new offset= %"PRId64"\n",
                       delta, input_files[ist->file_index]->ts_offset);
                pkt.dts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
                if (pkt.pts != AV_NOPTS_VALUE)
                    pkt.pts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
            }
        }

        // fprintf(stderr,"read #%d.%d size=%d\n", ist->file_index, ist->st->index, pkt.size);
        if (output_packet(ist, &pkt) < 0 || poll_filters() < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error while decoding stream #%d:%d\n",
                   ist->file_index, ist->st->index);
            if (exit_on_error)
                exit_program(1);
            av_free_packet(&pkt);
            continue;
        }

    discard_packet:
        av_free_packet(&pkt);

        /
        print_report(0, timer_start);
    }

    /
    for (i = 0; i < nb_input_streams; i++) {
        ist = input_streams[i];
        if (!input_files[ist->file_index]->eof_reached && ist->decoding_needed) {
            output_packet(ist, NULL);
        }
    }
    poll_filters();
    flush_encoders();

    term_exit();

    /
    for (i = 0; i < nb_output_files; i++) {
        os = output_files[i]->ctx;
        av_write_trailer(os);
    }

    /
    print_report(1, timer_start);

    /
    for (i = 0; i < nb_output_streams; i++) {
        ost = output_streams[i];
        if (ost->encoding_needed) {
            av_freep(&ost->st->codec->stats_in);
            avcodec_close(ost->st->codec);
        }
    }

    /
    for (i = 0; i < nb_input_streams; i++) {
        ist = input_streams[i];
        if (ist->decoding_needed) {
            avcodec_close(ist->st->codec);
        }
    }

    /
    ret = 0;

 fail:
    av_freep(&no_packet);

    if (output_streams) {
        for (i = 0; i < nb_output_streams; i++) {
            ost = output_streams[i];
            if (ost) {
                if (ost->stream_copy)
                    av_freep(&ost->st->codec->extradata);
                if (ost->logfile) {
                    fclose(ost->logfile);
                    ost->logfile = NULL;
                }
                av_fifo_free(ost->fifo); /
                av_freep(&ost->st->codec->subtitle_header);
                av_free(ost->forced_kf_pts);
                if (ost->avr)
                    avresample_free(&ost->avr);
                av_dict_free(&ost->opts);
            }
        }
    }
    return ret;
}
