static void mov_write_uuidprof_tag(AVIOContext *pb, AVFormatContext *s)
{
    AVStream       *video_st    = s->streams[0];
    AVCodecParameters *video_par = s->streams[0]->codecpar;
    AVCodecParameters *audio_par = s->streams[1]->codecpar;
    int audio_rate = audio_par->sample_rate;
    // TODO: should be avg_frame_rate
    int frame_rate = ((video_st->time_base.den) * (0x10000)) / (video_st->time_base.num);
    int audio_kbitrate = audio_par->bit_rate / 1000;
    int video_kbitrate = FFMIN(video_par->bit_rate / 1000, 800 - audio_kbitrate);

    avio_wb32(pb, 0x94); /
    ffio_wfourcc(pb, "uuid");
    ffio_wfourcc(pb, "PROF");

    avio_wb32(pb, 0x21d24fce); /
    avio_wb32(pb, 0xbb88695c);
    avio_wb32(pb, 0xfac9c740);

    avio_wb32(pb, 0x0);  /
    avio_wb32(pb, 0x3);  /

    avio_wb32(pb, 0x14); /
    ffio_wfourcc(pb, "FPRF");
    avio_wb32(pb, 0x0);  /
    avio_wb32(pb, 0x0);  /
    avio_wb32(pb, 0x0);  /

    avio_wb32(pb, 0x2c);  /
    ffio_wfourcc(pb, "APRF"); /
    avio_wb32(pb, 0x0);
    avio_wb32(pb, 0x2);   /
    ffio_wfourcc(pb, "mp4a");
    avio_wb32(pb, 0x20f);
    avio_wb32(pb, 0x0);
    avio_wb32(pb, audio_kbitrate);
    avio_wb32(pb, audio_kbitrate);
    avio_wb32(pb, audio_rate);
    avio_wb32(pb, audio_par->channels);

    avio_wb32(pb, 0x34);  /
    ffio_wfourcc(pb, "VPRF");   /
    avio_wb32(pb, 0x0);
    avio_wb32(pb, 0x1);    /
    if (video_par->codec_id == AV_CODEC_ID_H264) {
        ffio_wfourcc(pb, "avc1");
        avio_wb16(pb, 0x014D);
        avio_wb16(pb, 0x0015);
    } else {
        ffio_wfourcc(pb, "mp4v");
        avio_wb16(pb, 0x0000);
        avio_wb16(pb, 0x0103);
    }
    avio_wb32(pb, 0x0);
    avio_wb32(pb, video_kbitrate);
    avio_wb32(pb, video_kbitrate);
    avio_wb32(pb, frame_rate);
    avio_wb32(pb, frame_rate);
    avio_wb16(pb, video_par->width);
    avio_wb16(pb, video_par->height);
    avio_wb32(pb, 0x010001); /
}
