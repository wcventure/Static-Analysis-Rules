static void nal_send(AVFormatContext *s1, const uint8_t *buf, int size, int last)
{
    RTPMuxContext *s = s1->priv_data;

    av_log(s1, AV_LOG_DEBUG, "Sending NAL %x of len %d M=%d\n", buf[0] & 0x1F, size, last);
    if (size <= s->max_payload_size) {
        int buffered_size = s->buf_ptr - s->buf;
        // Flush buffered NAL units if the current unit doesn't fit
        if (buffered_size + 2 + size > s->max_payload_size) {
            flush_buffered(s1, 0);
            buffered_size = 0;
        }
        // If we aren't using mode 0, and the NAL unit fits including the
        // framing (2 bytes length, plus 1 byte for the STAP-A marker),
        // write the unit to the buffer as a STAP-A packet, otherwise flush
        // and send as single NAL.
        if (buffered_size + 3 + size <= s->max_payload_size &&
            !(s->flags & FF_RTP_FLAG_H264_MODE0)) {
            if (buffered_size == 0)
                *s->buf_ptr++ = 24;
            AV_WB16(s->buf_ptr, size);
            s->buf_ptr += 2;
            memcpy(s->buf_ptr, buf, size);
            s->buf_ptr += size;
            s->buffered_nals++;
        } else {
            flush_buffered(s1, 0);
            ff_rtp_send_data(s1, buf, size, last);
        }
    } else {
        uint8_t type = buf[0] & 0x1F;
        uint8_t nri = buf[0] & 0x60;

        flush_buffered(s1, 0);
        if (s->flags & FF_RTP_FLAG_H264_MODE0) {
            av_log(s1, AV_LOG_ERROR,
                   "NAL size %d > %d, try -slice-max-size %d\n", size,
                   s->max_payload_size, s->max_payload_size);
            return;
        }
        av_log(s1, AV_LOG_DEBUG, "NAL size %d > %d\n", size, s->max_payload_size);
        s->buf[0] = 28;        /
        s->buf[0] |= nri;
        s->buf[1] = type;
        s->buf[1] |= 1 << 7;
        buf += 1;
        size -= 1;
        while (size + 2 > s->max_payload_size) {
            memcpy(&s->buf[2], buf, s->max_payload_size - 2);
            ff_rtp_send_data(s1, s->buf, s->max_payload_size, 0);
            buf += s->max_payload_size - 2;
            size -= s->max_payload_size - 2;
            s->buf[1] &= ~(1 << 7);
        }
        s->buf[1] |= 1 << 6;
        memcpy(&s->buf[2], buf, size);
        ff_rtp_send_data(s1, s->buf, size + 2, last);
    }
}
