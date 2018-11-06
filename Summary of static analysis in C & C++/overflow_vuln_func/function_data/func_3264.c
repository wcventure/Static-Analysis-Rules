static inline uint64_t v4l2_get_pts(V4L2Buffer *avbuf)
{
    V4L2m2mContext *s = buf_to_m2mctx(avbuf);
    AVRational v4l2_timebase = { 1, USEC_PER_SEC };
    int64_t v4l2_pts;

    /
    v4l2_pts = avbuf->buf.timestamp.tv_sec * USEC_PER_SEC + avbuf->buf.timestamp.tv_usec;

    return av_rescale_q(v4l2_pts, v4l2_timebase, s->avctx->time_base);
}
