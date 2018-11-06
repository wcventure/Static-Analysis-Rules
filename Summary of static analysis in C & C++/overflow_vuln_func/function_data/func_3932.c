static int smacker_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    SmackerContext *smk = s->priv_data;
    int flags;
    int ret;
    int i;
    int frame_size = 0;
    int palchange = 0;

    if (s->pb->eof_reached || smk->cur_frame >= smk->frames)
        return AVERROR_EOF;

    /
    if(smk->curstream < 0) {
        avio_seek(s->pb, smk->nextpos, 0);
        frame_size = smk->frm_size[smk->cur_frame] & (~3);
        flags = smk->frm_flags[smk->cur_frame];
        /
        if(flags & SMACKER_PAL){
            int size, sz, t, off, j, pos;
            uint8_t *pal = smk->pal;
            uint8_t oldpal[768];

            memcpy(oldpal, pal, 768);
            size = avio_r8(s->pb);
            size = size * 4 - 1;
            frame_size -= size;
            frame_size--;
            sz = 0;
            pos = avio_tell(s->pb) + size;
            while(sz < 256){
                t = avio_r8(s->pb);
                if(t & 0x80){ /
                    sz += (t & 0x7F) + 1;
                    pal += ((t & 0x7F) + 1) * 3;
                } else if(t & 0x40){ /
                    off = avio_r8(s->pb);
                    j = (t & 0x3F) + 1;
                    if (off + j > 0xff) {
                        av_log(s, AV_LOG_ERROR,
                               "Invalid palette update, offset=%d length=%d extends beyond palette size\n",
                               off, j);
                        return AVERROR_INVALIDDATA;
                    }
                    off *= 3;
                    while(j-- && sz < 256) {
                        *pal++ = oldpal[off + 0];
                        *pal++ = oldpal[off + 1];
                        *pal++ = oldpal[off + 2];
                        sz++;
                        off += 3;
                    }
                } else { /
                    *pal++ = smk_pal[t];
                    *pal++ = smk_pal[avio_r8(s->pb) & 0x3F];
                    *pal++ = smk_pal[avio_r8(s->pb) & 0x3F];
                    sz++;
                }
            }
            avio_seek(s->pb, pos, 0);
            palchange |= 1;
        }
        flags >>= 1;
        smk->curstream = -1;
        /
        for(i = 0; i < 7; i++) {
            if(flags & 1) {
                int size;
                uint8_t *tmpbuf;

                size = avio_rl32(s->pb) - 4;
                frame_size -= size;
                frame_size -= 4;
                smk->curstream++;
                tmpbuf = av_realloc(smk->bufs[smk->curstream], size);
                if (!tmpbuf)
                    return AVERROR(ENOMEM);
                smk->bufs[smk->curstream] = tmpbuf;
                smk->buf_sizes[smk->curstream] = size;
                ret = avio_read(s->pb, smk->bufs[smk->curstream], size);
                if(ret != size)
                    return AVERROR(EIO);
                smk->stream_id[smk->curstream] = smk->indexes[i];
            }
            flags >>= 1;
        }
        if (frame_size < 0)
            return AVERROR_INVALIDDATA;
        if (av_new_packet(pkt, frame_size + 769))
            return AVERROR(ENOMEM);
        if(smk->frm_size[smk->cur_frame] & 1)
            palchange |= 2;
        pkt->data[0] = palchange;
        memcpy(pkt->data + 1, smk->pal, 768);
        ret = avio_read(s->pb, pkt->data + 769, frame_size);
        if(ret != frame_size)
            return AVERROR(EIO);
        pkt->stream_index = smk->videoindex;
        pkt->size = ret + 769;
        smk->cur_frame++;
        smk->nextpos = avio_tell(s->pb);
    } else {
        if (av_new_packet(pkt, smk->buf_sizes[smk->curstream]))
            return AVERROR(ENOMEM);
        memcpy(pkt->data, smk->bufs[smk->curstream], smk->buf_sizes[smk->curstream]);
        pkt->size = smk->buf_sizes[smk->curstream];
        pkt->stream_index = smk->stream_id[smk->curstream];
        pkt->pts = smk->aud_pts[smk->curstream];
        smk->aud_pts[smk->curstream] += AV_RL32(pkt->data);
        smk->curstream--;
    }

    return 0;
}
