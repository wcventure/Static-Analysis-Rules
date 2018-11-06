static int wav_read_header(AVFormatContext *s)
{
    int64_t size, av_uninit(data_size);
    int64_t sample_count = 0;
    int rf64;
    uint32_t tag;
    AVIOContext *pb      = s->pb;
    AVStream *st         = NULL;
    WAVDemuxContext *wav = s->priv_data;
    int ret, got_fmt = 0;
    int64_t next_tag_ofs, data_ofs = -1;

    wav->smv_data_ofs = -1;

    /
    tag = avio_rl32(pb);

    rf64 = tag == MKTAG('R', 'F', '6', '4');
    if (!rf64 && tag != MKTAG('R', 'I', 'F', 'F'))
        return AVERROR_INVALIDDATA;
    avio_rl32(pb); /
    tag = avio_rl32(pb);
    if (tag != MKTAG('W', 'A', 'V', 'E'))
        return AVERROR_INVALIDDATA;

    if (rf64) {
        if (avio_rl32(pb) != MKTAG('d', 's', '6', '4'))
            return AVERROR_INVALIDDATA;
        size = avio_rl32(pb);
        if (size < 24)
            return AVERROR_INVALIDDATA;
        avio_rl64(pb); /

        data_size    = avio_rl64(pb);
        sample_count = avio_rl64(pb);

        if (data_size < 0 || sample_count < 0) {
            av_log(s, AV_LOG_ERROR, "negative data_size and/or sample_count in "
                   "ds64: data_size = %"PRId64", sample_count = %"PRId64"\n",
                   data_size, sample_count);
            return AVERROR_INVALIDDATA;
        }
        avio_skip(pb, size - 24); /

    }

    for (;;) {
        AVStream *vst;
        size         = next_tag(pb, &tag);
        next_tag_ofs = avio_tell(pb) + size;

        if (url_feof(pb))
            break;

        switch (tag) {
        case MKTAG('f', 'm', 't', ' '):
            /
            if (!got_fmt && (ret = wav_parse_fmt_tag(s, size, &st)) < 0) {
                return ret;
            } else if (got_fmt)
                av_log(s, AV_LOG_WARNING, "found more than one 'fmt ' tag\n");

            got_fmt = 1;
            break;
        case MKTAG('d', 'a', 't', 'a'):
            if (!got_fmt) {
                av_log(s, AV_LOG_ERROR,
                       "found no 'fmt ' tag before the 'data' tag\n");
                return AVERROR_INVALIDDATA;
            }

            if (rf64) {
                next_tag_ofs = wav->data_end = avio_tell(pb) + data_size;
            } else {
                data_size    = size;
                next_tag_ofs = wav->data_end = size ? next_tag_ofs : INT64_MAX;
            }

            data_ofs = avio_tell(pb);

            /
            if (!pb->seekable || (!rf64 && !size))
                goto break_loop;
            break;
        case MKTAG('f', 'a', 'c', 't'):
            if (!sample_count)
                sample_count = avio_rl32(pb);
            break;
        case MKTAG('b', 'e', 'x', 't'):
            if ((ret = wav_parse_bext_tag(s, size)) < 0)
                return ret;
            break;
        case MKTAG('S','M','V','0'):
            if (!got_fmt) {
                av_log(s, AV_LOG_ERROR, "found no 'fmt ' tag before the 'SMV0' tag\n");
                return AVERROR_INVALIDDATA;
            }
            // SMV file, a wav file with video appended.
            if (size != MKTAG('0','2','0','0')) {
                av_log(s, AV_LOG_ERROR, "Unknown SMV version found\n");
                goto break_loop;
            }
            av_log(s, AV_LOG_DEBUG, "Found SMV data\n");
            vst = avformat_new_stream(s, NULL);
            if (!vst)
                return AVERROR(ENOMEM);
            avio_r8(pb);
            vst->id = 1;
            vst->codec->codec_type = AVMEDIA_TYPE_VIDEO;
            vst->codec->codec_id = AV_CODEC_ID_MJPEG;
            vst->codec->width  = avio_rl24(pb);
            vst->codec->height = avio_rl24(pb);
            size = avio_rl24(pb);
            wav->smv_data_ofs = avio_tell(pb) + (size - 5) * 3;
            avio_rl24(pb);
            wav->smv_block_size = avio_rl24(pb);
            avpriv_set_pts_info(vst, 32, 1, avio_rl24(pb));
            vst->duration = avio_rl24(pb);
            avio_rl24(pb);
            avio_rl24(pb);
            wav->smv_frames_per_jpeg = avio_rl24(pb);
            goto break_loop;
        case MKTAG('L', 'I', 'S', 'T'):
            if (size < 4) {
                av_log(s, AV_LOG_ERROR, "too short LIST tag\n");
                return AVERROR_INVALIDDATA;
            }
            switch (avio_rl32(pb)) {
            case MKTAG('I', 'N', 'F', 'O'):
                ff_read_riff_info(s, size - 4);
            }
            break;
        }

        /
        next_tag_ofs += (next_tag_ofs < INT64_MAX && next_tag_ofs & 1);

        /
        if ((avio_size(pb) > 0 && next_tag_ofs >= avio_size(pb)) ||
            wav_seek_tag(pb, next_tag_ofs, SEEK_SET) < 0) {
            break;
        }
    }

break_loop:
    if (data_ofs < 0) {
        av_log(s, AV_LOG_ERROR, "no 'data' tag found\n");
        return AVERROR_INVALIDDATA;
    }

    avio_seek(pb, data_ofs, SEEK_SET);

    if (!sample_count && st->codec->channels &&
        av_get_bits_per_sample(st->codec->codec_id) && wav->data_end <= avio_size(pb))
        sample_count = (data_size << 3) /
                       (st->codec->channels *
                        (uint64_t)av_get_bits_per_sample(st->codec->codec_id));
    if (sample_count)
        st->duration = sample_count;

    ff_metadata_conv_ctx(s, NULL, wav_metadata_conv);
    ff_metadata_conv_ctx(s, NULL, ff_riff_info_conv);

    return 0;
}
