static int http_read_stream(URLContext *h, uint8_t *buf, int size)
{
    HTTPContext *s = h->priv_data;
    int err, new_location;

    if (!s->hd)
        return AVERROR_EOF;

    if (s->end_chunked_post && !s->end_header) {
        err = http_read_header(h, &new_location);
        if (err < 0)
            return err;
    }

    if (s->chunksize >= 0) {
        if (!s->chunksize) {
            char line[32];

            for (;;) {
                do {
                    if ((err = http_get_line(s, line, sizeof(line))) < 0)
                        return err;
                } while (!*line);    /

                s->chunksize = strtoll(line, NULL, 16);

                av_log(NULL, AV_LOG_TRACE, "Chunked encoding data size: %"PRId64"'\n",
                        s->chunksize);

                if (!s->chunksize)
                    return 0;
                break;
            }
        }
        size = FFMIN(size, s->chunksize);
    }
#if CONFIG_ZLIB
    if (s->compressed)
        return http_buf_read_compressed(h, buf, size);
#endif /
    return http_buf_read(h, buf, size);
}
