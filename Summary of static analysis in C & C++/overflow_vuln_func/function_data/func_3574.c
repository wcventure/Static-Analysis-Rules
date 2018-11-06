static av_always_inline
void clamp_mv(VP8Context *s, VP56mv *dst, const VP56mv *src)
{
    dst->x = av_clip(src->x, s->mv_min.x, s->mv_max.x);
    dst->y = av_clip(src->y, s->mv_min.y, s->mv_max.y);
}
