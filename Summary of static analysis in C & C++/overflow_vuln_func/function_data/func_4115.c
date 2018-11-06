static int nsv_read_chunk(AVFormatContext *s, int fill_header)
{
    NSVContext *nsv = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st[2] = {NULL, NULL};
    NSVStream *nst;
    AVPacket *pkt;
    int i, err = 0;
    uint8_t auxcount; /
    uint32_t vsize;
    uint16_t asize;
    uint16_t auxsize;
    uint32_t av_unused auxtag;

    av_dlog(s, "%s(%d)\n", __FUNCTION__, fill_header);

    if (nsv->ahead[0].data || nsv->ahead[1].data)
        return 0; //-1; /

null_chunk_retry:
    if (url_feof(pb))
        return -1;

    for (i = 0; i < NSV_MAX_RESYNC_TRIES && nsv->state < NSV_FOUND_NSVS && !err; i++)
        err = nsv_resync(s);
    if (err < 0)
        return err;
    if (nsv->state == NSV_FOUND_NSVS)
        err = nsv_parse_NSVs_header(s, NULL);
    if (err < 0)
        return err;
    if (nsv->state != NSV_HAS_READ_NSVS && nsv->state != NSV_FOUND_BEEF)
        return -1;

    auxcount = avio_r8(pb);
    vsize = avio_rl16(pb);
    asize = avio_rl16(pb);
    vsize = (vsize << 4) | (auxcount >> 4);
    auxcount &= 0x0f;
    av_dlog(s, "NSV CHUNK %d aux, %u bytes video, %d bytes audio\n", auxcount, vsize, asize);
    /
    for (i = 0; i < auxcount; i++) {
        auxsize = avio_rl16(pb);
        auxtag = avio_rl32(pb);
        av_dlog(s, "NSV aux data: '%c%c%c%c', %d bytes\n",
              (auxtag & 0x0ff),
              ((auxtag >> 8) & 0x0ff),
              ((auxtag >> 16) & 0x0ff),
              ((auxtag >> 24) & 0x0ff),
              auxsize);
        avio_skip(pb, auxsize);
        vsize -= auxsize + sizeof(uint16_t) + sizeof(uint32_t); /
    }

    if (url_feof(pb))
        return -1;
    if (!vsize && !asize) {
        nsv->state = NSV_UNSYNC;
        goto null_chunk_retry;
    }

    /
    if (s->streams[0])
        st[s->streams[0]->id] = s->streams[0];
    if (s->streams[1])
        st[s->streams[1]->id] = s->streams[1];

    if (vsize/) {
        nst = st[NSV_ST_VIDEO]->priv_data;
        pkt = &nsv->ahead[NSV_ST_VIDEO];
        av_get_packet(pb, pkt, vsize);
        pkt->stream_index = st[NSV_ST_VIDEO]->index;//NSV_ST_VIDEO;
        pkt->dts = nst->frame_offset;
        pkt->flags |= nsv->state == NSV_HAS_READ_NSVS ? AV_PKT_FLAG_KEY : 0; /
        for (i = 0; i < FFMIN(8, vsize); i++)
            av_dlog(s, "NSV video: [%d] = %02x\n", i, pkt->data[i]);
    }
    if(st[NSV_ST_VIDEO])
        ((NSVStream*)st[NSV_ST_VIDEO]->priv_data)->frame_offset++;

    if (asize/) {
        nst = st[NSV_ST_AUDIO]->priv_data;
        pkt = &nsv->ahead[NSV_ST_AUDIO];
        /
        /
        if (asize && st[NSV_ST_AUDIO]->codec->codec_tag == MKTAG('P', 'C', 'M', ' ')/) {
            uint8_t bps;
            uint8_t channels;
            uint16_t samplerate;
            bps = avio_r8(pb);
            channels = avio_r8(pb);
            samplerate = avio_rl16(pb);
            asize-=4;
            av_dlog(s, "NSV RAWAUDIO: bps %d, nchan %d, srate %d\n", bps, channels, samplerate);
            if (fill_header) {
                st[NSV_ST_AUDIO]->need_parsing = AVSTREAM_PARSE_NONE; /
                if (bps != 16) {
                    av_dlog(s, "NSV AUDIO bit/sample != 16 (%d)!!!\n", bps);
                }
                bps /= channels; // ???
                if (bps == 8)
                    st[NSV_ST_AUDIO]->codec->codec_id = CODEC_ID_PCM_U8;
                samplerate /= 4;/
                channels = 1;
                st[NSV_ST_AUDIO]->codec->channels = channels;
                st[NSV_ST_AUDIO]->codec->sample_rate = samplerate;
                av_dlog(s, "NSV RAWAUDIO: bps %d, nchan %d, srate %d\n", bps, channels, samplerate);
            }
        }
        av_get_packet(pb, pkt, asize);
        pkt->stream_index = st[NSV_ST_AUDIO]->index;//NSV_ST_AUDIO;
        pkt->flags |= nsv->state == NSV_HAS_READ_NSVS ? AV_PKT_FLAG_KEY : 0; /
        if( nsv->state == NSV_HAS_READ_NSVS && st[NSV_ST_VIDEO] ) {
            /
            pkt->dts = (((NSVStream*)st[NSV_ST_VIDEO]->priv_data)->frame_offset-1);
            pkt->dts *= (int64_t)1000        * nsv->framerate.den;
            pkt->dts += (int64_t)nsv->avsync * nsv->framerate.num;
            av_dlog(s, "NSV AUDIO: sync:%d, dts:%"PRId64, nsv->avsync, pkt->dts);
        }
        nst->frame_offset++;
    }

    nsv->state = NSV_UNSYNC;
    return 0;
}
