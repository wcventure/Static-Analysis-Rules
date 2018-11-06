static int mov_skip_multiple_stsd(MOVContext *c, AVIOContext *pb,
                                  int codec_tag, int format,
                                  int size)
{
    int video_codec_id = ff_codec_get_id(ff_codec_movvideo_tags, format);

    if (codec_tag &&
         (codec_tag != format &&
          (c->fc->video_codec_id ? video_codec_id != c->fc->video_codec_id
                                 : codec_tag != MKTAG('j','p','e','g')))) {
        /

        av_log(c->fc, AV_LOG_WARNING, "multiple fourcc not supported\n");
        avio_skip(pb, size);
        return 1;
    }
    if ( codec_tag == AV_RL32("avc1") ||
         codec_tag == AV_RL32("hvc1") ||
         codec_tag == AV_RL32("hev1")
    )
        av_log(c->fc, AV_LOG_WARNING, "Concatenated H.264 or H.265 might not play correctly.\n");

    return 0;
}
