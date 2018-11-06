FILE_T
file_fdopen(int fd)
{
#ifdef _STATBUF_ST_BLKSIZE      /
    ws_statb64 st;
#endif
    int want = GZBUFSIZE;
    FILE_T state;

    if (fd == -1)
        return NULL;

    /
    state = (FILE_T)g_try_malloc(sizeof *state);
    if (state == NULL)
        return NULL;

    state->fast_seek_cur = NULL;
    state->fast_seek = NULL;

    /
    state->fd = fd;

    /
    state->is_compressed = FALSE;

    /
    state->start = ws_lseek64(state->fd, 0, SEEK_CUR);
    if (state->start == -1) state->start = 0;
    state->raw_pos = state->start;

    /
    gz_reset(state);

#ifdef _STATBUF_ST_BLKSIZE
    if (ws_fstat64(fd, &st) >= 0) {
        /
        if (st.st_blksize <= G_MAXINT)
            want = (int)st.st_blksize;
        /
    }
#endif

    /
    state->in = (unsigned char *)g_try_malloc(want);
    state->out = (unsigned char *)g_try_malloc(want << 1);
    state->size = want;
    if (state->in == NULL || state->out == NULL) {
        g_free(state->out);
        g_free(state->in);
        g_free(state);
        errno = ENOMEM;
        return NULL;
    }

#ifdef HAVE_ZLIB
    /
    state->strm.zalloc = Z_NULL;
    state->strm.zfree = Z_NULL;
    state->strm.opaque = Z_NULL;
    state->strm.avail_in = 0;
    state->strm.next_in = Z_NULL;
    if (inflateInit2(&(state->strm), -15) != Z_OK) {    /
        g_free(state->out);
        g_free(state->in);
        g_free(state);
        errno = ENOMEM;
        return NULL;
    }

    /
    state->dont_check_crc = FALSE;
#endif
    /
    return state;
}
