static inline void comp_block(MadContext *t, int mb_x, int mb_y,
                              int j, int mv_x, int mv_y, int add)
{
    MpegEncContext *s = &t->s;
    if (j < 4) {
        comp(t->frame.data[0] + (mb_y*16 + ((j&2)<<2))*t->frame.linesize[0] + mb_x*16 + ((j&1)<<3),
             t->frame.linesize[0],
             t->last_frame.data[0] + (mb_y*16 + ((j&2)<<2) + mv_y)*t->last_frame.linesize[0] + mb_x*16 + ((j&1)<<3) + mv_x,
             t->last_frame.linesize[0], add);
    } else if (!(s->avctx->flags & CODEC_FLAG_GRAY)) {
        int index = j - 3;
        comp(t->frame.data[index] + (mb_y*8)*t->frame.linesize[index] + mb_x * 8,
             t->frame.linesize[index],
             t->last_frame.data[index] + (mb_y * 8 + (mv_y/2))*t->last_frame.linesize[index] + mb_x * 8 + (mv_x/2),
             t->last_frame.linesize[index], add);
    }
}
