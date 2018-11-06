static int dvbsub_parse(AVCodecParserContext *s,
                        AVCodecContext *avctx,
                        const uint8_t **poutbuf, int *poutbuf_size,
                        const uint8_t *buf, int buf_size)
{
    DVBSubParseContext *pc = s->priv_data;
    uint8_t *p, *p_end;
    int i, len, buf_pos = 0;

    av_dlog(avctx, "DVB parse packet pts=%"PRIx64", lpts=%"PRIx64", cpts=%"PRIx64":\n",
            s->pts, s->last_pts, s->cur_frame_pts[s->cur_frame_start_index]);

    for (i=0; i < buf_size; i++)
    {
        av_dlog(avctx, "%02x ", buf[i]);
        if (i % 16 == 15)
            av_dlog(avctx, "\n");
    }

    if (i % 16 != 0)
        av_dlog(avctx, "\n");

    *poutbuf = NULL;
    *poutbuf_size = 0;

    s->fetch_timestamp = 1;

    if (s->last_pts != s->pts && s->pts != AV_NOPTS_VALUE) /
    {
        if (pc->packet_index != pc->packet_start)
        {
            av_dlog(avctx, "Discarding %d bytes\n",
                    pc->packet_index - pc->packet_start);
        }

        pc->packet_start = 0;
        pc->packet_index = 0;

        if (buf_size < 2 || buf[0] != 0x20 || buf[1] != 0x00) {
            av_dlog(avctx, "Bad packet header\n");
            return -1;
        }

        buf_pos = 2;

        pc->in_packet = 1;
    } else {
        if (pc->packet_start != 0)
        {
            if (pc->packet_index != pc->packet_start)
            {
                memmove(pc->packet_buf, pc->packet_buf + pc->packet_start,
                            pc->packet_index - pc->packet_start);

                pc->packet_index -= pc->packet_start;
                pc->packet_start = 0;
            } else {
                pc->packet_start = 0;
                pc->packet_index = 0;
            }
        }
    }

    if (buf_size - buf_pos + pc->packet_index > PARSE_BUF_SIZE)
        return -1;

/
    if (pc->in_packet == 0)
        return buf_size;

    memcpy(pc->packet_buf + pc->packet_index, buf + buf_pos, buf_size - buf_pos);
    pc->packet_index += buf_size - buf_pos;

    p = pc->packet_buf;
    p_end = pc->packet_buf + pc->packet_index;

    while (p < p_end)
    {
        if (*p == 0x0f)
        {
            if (p + 6 <= p_end)
            {
                len = AV_RB16(p + 4);

                if (p + len + 6 <= p_end)
                {
                    *poutbuf_size += len + 6;

                    p += len + 6;
                } else
                    break;
            } else
                break;
        } else if (*p == 0xff) {
            if (p + 1 < p_end)
            {
                av_dlog(avctx, "Junk at end of packet\n");
            }
            pc->packet_index = p - pc->packet_buf;
            pc->in_packet = 0;
            break;
        } else {
            av_log(avctx, AV_LOG_ERROR, "Junk in packet\n");

            pc->packet_index = p - pc->packet_buf;
            pc->in_packet = 0;
            break;
        }
    }

    if (*poutbuf_size > 0)
    {
        *poutbuf = pc->packet_buf;
        pc->packet_start = *poutbuf_size;
    }

    if (s->pts == AV_NOPTS_VALUE)
        s->pts = s->last_pts;

    return buf_size;
}
