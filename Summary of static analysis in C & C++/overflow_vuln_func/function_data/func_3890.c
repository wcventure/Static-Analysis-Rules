    c->buffer_ptr = c->buffer;
    c->buffer_end = c->buffer + FFM_PACKET_SIZE;
    c->stream->feed_opened = 1;
    return 0;
}
    
static int http_receive_data(HTTPContext *c)
{
    int len;
    HTTPContext *c1;

    if (c->buffer_ptr >= c->buffer_end) {
        FFStream *feed = c->stream;
        /
        if (c->data_count > FFM_PACKET_SIZE) {
            
            //            printf("writing pos=0x%Lx size=0x%Lx\n", feed->feed_write_index, feed->feed_size);
            /
            lseek(c->feed_fd, feed->feed_write_index, SEEK_SET);
            write(c->feed_fd, c->buffer, FFM_PACKET_SIZE);
            
            feed->feed_write_index += FFM_PACKET_SIZE;
            /
            if (feed->feed_write_index > c->stream->feed_size)
                feed->feed_size = feed->feed_write_index;

            /
            if (feed->feed_write_index >= c->stream->feed_max_size)
                feed->feed_write_index = FFM_PACKET_SIZE;

            /
            ffm_write_write_index(c->feed_fd, feed->feed_write_index);

            /
            for(c1 = first_http_ctx; c1 != NULL; c1 = c1->next) {
                if (c1->state == HTTPSTATE_WAIT_FEED && 
                    c1->stream->feed == c->stream->feed) {
                    c1->state = HTTPSTATE_SEND_DATA;
                }
            }
        } else {
            /
            AVFormatContext s;
            ByteIOContext *pb = &s.pb;
            int i;

            memset(&s, 0, sizeof(s));

            url_open_buf(pb, c->buffer, c->buffer_end - c->buffer, URL_RDONLY);
            pb->buf_end = c->buffer_end;        /
            pb->is_streamed = 1;

            if (feed->fmt->read_header(&s, 0) < 0) {
                goto fail;
            }

            /
            if (s.nb_streams != feed->nb_streams) {
                goto fail;
            }
            for (i = 0; i < s.nb_streams; i++) {
                memcpy(&feed->streams[i]->codec, &s.streams[i]->codec, sizeof(AVCodecContext));
            } 
        }
        c->buffer_ptr = c->buffer;
    }

    len = read(c->fd, c->buffer_ptr, c->buffer_end - c->buffer_ptr);
    if (len < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            /
            goto fail;
        }
    } else if (len == 0) {
        /
        goto fail;
    } else {
        c->buffer_ptr += len;
        c->data_count += len;
    }
    return 0;
 fail:
    c->stream->feed_opened = 0;
    close(c->feed_fd);
    return -1;
}
