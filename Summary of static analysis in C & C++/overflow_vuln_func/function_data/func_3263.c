static int decode_frame(AVCodecContext * avctx,
			void *data, int *data_size,
			UINT8 * buf, int buf_size)
{
    MPADecodeContext *s = avctx->priv_data;
    UINT32 header;
    UINT8 *buf_ptr;
    int len, out_size;
    short *out_samples = data;

    *data_size = 0;
    buf_ptr = buf;
    while (buf_size > 0) {
	len = s->inbuf_ptr - s->inbuf;
	if (s->frame_size == 0) {
            /
            if (s->free_format_next_header != 0) {
                s->inbuf[0] = s->free_format_next_header >> 24;
                s->inbuf[1] = s->free_format_next_header >> 16;
                s->inbuf[2] = s->free_format_next_header >> 8;
                s->inbuf[3] = s->free_format_next_header;
                s->inbuf_ptr = s->inbuf + 4;
                s->free_format_next_header = 0;
                goto got_header;
            }
	    /
	    len = HEADER_SIZE - len;
	    if (len > buf_size)
		len = buf_size;
	    if (len > 0) {
		memcpy(s->inbuf_ptr, buf_ptr, len);
		buf_ptr += len;
		buf_size -= len;
		s->inbuf_ptr += len;
	    }
	    if ((s->inbuf_ptr - s->inbuf) >= HEADER_SIZE) {
            got_header:
		header = (s->inbuf[0] << 24) | (s->inbuf[1] << 16) |
		    (s->inbuf[2] << 8) | s->inbuf[3];

		if (check_header(header) < 0) {
		    /
		    memcpy(s->inbuf, s->inbuf + 1, s->inbuf_ptr - s->inbuf - 1);
		    s->inbuf_ptr--;
                    dprintf("skip %x\n", header);
                    /
                    s->free_format_frame_size = 0;
		} else {
		    if (decode_header(s, header) == 1) {
                        /
			s->frame_size = -1;
			memcpy(s->inbuf, s->inbuf + 1, s->inbuf_ptr - s->inbuf - 1);
			s->inbuf_ptr--;
                    } else {
                        /
                        avctx->sample_rate = s->sample_rate;
                        avctx->channels = s->nb_channels;
			avctx->bit_rate = s->bit_rate;
			avctx->frame_size = s->frame_size;
                    }
		}
	    }
        } else if (s->frame_size == -1) {
            /
	    len = MPA_MAX_CODED_FRAME_SIZE - len;
	    if (len > buf_size)
		len = buf_size;
            if (len == 0) {
                /
                s->frame_size = 0;
            } else {
                UINT8 *p, *pend;
                UINT32 header1;
                int padding;

                memcpy(s->inbuf_ptr, buf_ptr, len);
                /
                p = s->inbuf_ptr - 3;
                pend = s->inbuf_ptr + len - 4;
                while (p <= pend) {
                    header = (p[0] << 24) | (p[1] << 16) |
                        (p[2] << 8) | p[3];
                    header1 = (s->inbuf[0] << 24) | (s->inbuf[1] << 16) |
                        (s->inbuf[2] << 8) | s->inbuf[3];
                    /
                    if ((header & SAME_HEADER_MASK) ==
                        (header1 & SAME_HEADER_MASK)) {
                        /
                        len = (p + 4) - s->inbuf_ptr;
                        buf_ptr += len;
                        buf_size -= len;
                        s->inbuf_ptr = p;
                        /
                        s->free_format_next_header = header;
                        s->free_format_frame_size = s->inbuf_ptr - s->inbuf;
                        padding = (header1 >> 9) & 1;
                        if (s->layer == 1)
                            s->free_format_frame_size -= padding * 4;
                        else
                            s->free_format_frame_size -= padding;
                        dprintf("free frame size=%d padding=%d\n", 
                                s->free_format_frame_size, padding);
                        decode_header(s, header1);
                        goto next_data;
                    }
                    p++;
                }
                /
                buf_ptr += len;
                s->inbuf_ptr += len;
                buf_size -= len;
            }
	} else if (len < s->frame_size) {
            if (s->frame_size > MPA_MAX_CODED_FRAME_SIZE)
                s->frame_size = MPA_MAX_CODED_FRAME_SIZE;
	    len = s->frame_size - len;
	    if (len > buf_size)
		len = buf_size;
	    memcpy(s->inbuf_ptr, buf_ptr, len);
	    buf_ptr += len;
	    s->inbuf_ptr += len;
	    buf_size -= len;
	} else {
            out_size = mp_decode_frame(s, out_samples);
	    s->inbuf_ptr = s->inbuf;
	    s->frame_size = 0;
	    *data_size = out_size;
	    break;
	}
    next_data:
    }
    return buf_ptr - buf;
}
