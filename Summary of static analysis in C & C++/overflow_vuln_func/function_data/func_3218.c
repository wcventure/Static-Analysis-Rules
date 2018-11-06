int64_t ff_lsb2full(StreamContext *stream, int64_t lsb){
    int64_t mask = (1<<stream->msb_pts_shift)-1;
    int64_t delta= stream->last_pts - mask/2;
    return  ((lsb - delta)&mask) + delta;
}
