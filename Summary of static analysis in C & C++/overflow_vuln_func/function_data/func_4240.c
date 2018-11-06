static int opt_new_stream(const char *opt, const char *arg)
{
    AVFormatContext *oc;
    if (nb_output_files <= 0) {
        fprintf(stderr, "At least one output file must be specified\n");
        ffmpeg_exit(1);
    }
    oc = output_files[nb_output_files - 1];

    if      (!strcmp(opt, "newvideo"   )) new_video_stream   (oc);
    else if (!strcmp(opt, "newaudio"   )) new_audio_stream   (oc);
    else if (!strcmp(opt, "newsubtitle")) new_subtitle_stream(oc);
    else av_assert0(0);
    return 0;
}
