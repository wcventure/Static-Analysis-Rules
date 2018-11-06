static int buffer_needs_copy(PadContext *s, AVFrame *frame, AVBufferRef *buf)
{
    int planes[4] = { -1, -1, -1, -1}, *p = planes;
    int i, j;

    /
    for (i = 0; i < FF_ARRAY_ELEMS(planes) && frame->data[i]; i++) {
        if (av_frame_get_plane_buffer(frame, i) == buf)
            *p++ = i;
    }

    /
    for (i = 0; i < FF_ARRAY_ELEMS(planes) && planes[i] >= 0; i++) {
        int hsub = s->draw.hsub[planes[i]];
        int vsub = s->draw.vsub[planes[i]];

        uint8_t *start = frame->data[planes[i]];
        uint8_t *end   = start + (frame->height >> hsub) *
                                 frame->linesize[planes[i]];

        /
        ptrdiff_t req_start = (s->x >> hsub) * s->draw.pixelstep[planes[i]] +
                              (s->y >> vsub) * frame->linesize[planes[i]];
        ptrdiff_t req_end   = ((s->w - s->x - frame->width) >> hsub) *
                              s->draw.pixelstep[planes[i]] +
                              (s->y >> vsub) * frame->linesize[planes[i]];

        if (frame->linesize[planes[i]] < (s->w >> hsub) * s->draw.pixelstep[planes[i]])
            return 1;
        if (start - buf->data < req_start ||
            (buf->data + buf->size) - end < req_end)
            return 1;

#define SIGN(x) ((x) > 0 ? 1 : -1)
        for (j = 0; j < FF_ARRAY_ELEMS(planes) & planes[j] >= 0; j++) {
            int hsub1 = s->draw.hsub[planes[j]];
            uint8_t *start1 = frame->data[planes[j]];
            uint8_t *end1   = start1 + (frame->height >> hsub1) *
                                       frame->linesize[planes[j]];
            if (i == j)
                continue;

            if (SIGN(start - end1) != SIGN(start - end1 - req_start) ||
                SIGN(end - start1) != SIGN(end - start1 + req_end))
                return 1;
        }
    }

    return 0;
}
