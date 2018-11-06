static int send_invoke_response(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    double seqnum;
    char filename[64];
    char command[64];
    int stringlen;
    char *pchar;
    const uint8_t *p = pkt->data;
    uint8_t *pp      = NULL;
    RTMPPacket spkt  = { 0 };
    GetByteContext gbc;
    int ret;

    bytestream2_init(&gbc, p, pkt->size);
    if (ff_amf_read_string(&gbc, command, sizeof(command),
                           &stringlen)) {
        av_log(s, AV_LOG_ERROR, "Error in PT_INVOKE\n");
        return AVERROR_INVALIDDATA;
    }

    ret = ff_amf_read_number(&gbc, &seqnum);
    if (ret)
        return ret;
    ret = ff_amf_read_null(&gbc);
    if (ret)
        return ret;
    if (!strcmp(command, "FCPublish") ||
        !strcmp(command, "publish")) {
        ret = ff_amf_read_string(&gbc, filename,
                                 sizeof(filename), &stringlen);
        if (ret) {
            if (ret == AVERROR(EINVAL))
                av_log(s, AV_LOG_ERROR, "Unable to parse stream name - name too long?\n");
            else
                av_log(s, AV_LOG_ERROR, "Unable to parse stream name\n");
            return ret;
        }
        // check with url
        if (s->filename) {
            pchar = strrchr(s->filename, '/');
            if (!pchar) {
                av_log(s, AV_LOG_WARNING,
                       "Unable to find / in url %s, bad format\n",
                       s->filename);
                pchar = s->filename;
            }
            pchar++;
            if (strcmp(pchar, filename))
                av_log(s, AV_LOG_WARNING, "Unexpected stream %s, expecting"
                       " %s\n", filename, pchar);
        }
        rt->state = STATE_RECEIVING;
    }

    if (!strcmp(command, "FCPublish")) {
        if ((ret = ff_rtmp_packet_create(&spkt, RTMP_SYSTEM_CHANNEL,
                                         RTMP_PT_INVOKE, 0,
                                         RTMP_PKTDATA_DEFAULT_SIZE)) < 0) {
            av_log(s, AV_LOG_ERROR, "Unable to create response packet\n");
            return ret;
        }
        pp = spkt.data;
        ff_amf_write_string(&pp, "onFCPublish");
    } else if (!strcmp(command, "publish")) {
        ret = write_begin(s);
        if (ret < 0)
            return ret;

        // Send onStatus(NetStream.Publish.Start)
        return write_status(s, pkt, "NetStream.Publish.Start",
                           filename);
    } else if (!strcmp(command, "play")) {
        ret = write_begin(s);
        if (ret < 0)
            return ret;
        rt->state = STATE_SENDING;
        return write_status(s, pkt, "NetStream.Play.Start",
                            filename);
    } else {
        if ((ret = ff_rtmp_packet_create(&spkt, RTMP_SYSTEM_CHANNEL,
                                         RTMP_PT_INVOKE, 0,
                                         RTMP_PKTDATA_DEFAULT_SIZE)) < 0) {
            av_log(s, AV_LOG_ERROR, "Unable to create response packet\n");
            return ret;
        }
        pp = spkt.data;
        ff_amf_write_string(&pp, "_result");
        ff_amf_write_number(&pp, seqnum);
        ff_amf_write_null(&pp);
        if (!strcmp(command, "createStream")) {
            rt->nb_streamid++;
            if (rt->nb_streamid == 0 || rt->nb_streamid == 2)
                rt->nb_streamid++; /
            ff_amf_write_number(&pp, rt->nb_streamid);
            /
        }
    }
    spkt.size = pp - spkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &spkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&spkt);
    return ret;
}
