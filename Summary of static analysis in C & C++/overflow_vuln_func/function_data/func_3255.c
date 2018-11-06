AVStream *add_video_stream(AVFormatContext *oc, int codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }
    
    c = &st->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;

    /
    c->bit_rate = 400000;
    /
    c->width = 352;  
    c->height = 288;
    /
    c->frame_rate = STREAM_FRAME_RATE;  
    c->frame_rate_base = 1;
    c->gop_size = 12; /
    if (c->codec_id == CODEC_ID_MPEG1VIDEO ||
        c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /
        c->max_b_frames = 2;
    }
    // some formats want stream headers to be seperate
    if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name, "3gp"))
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    
    return st;
}
