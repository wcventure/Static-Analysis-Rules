static inline struct rgbvec interp_trilinear(const LUT3DContext *lut3d,
                                             const struct rgbvec *s)
{
    const struct rgbvec d = {s->r - PREV(s->r), s->g - PREV(s->g), s->b - PREV(s->b)};
    const struct rgbvec c000 = lut3d->lut[PREV(s->r)][PREV(s->g)][PREV(s->b)];
    const struct rgbvec c001 = lut3d->lut[PREV(s->r)][PREV(s->g)][NEXT(s->b)];
    const struct rgbvec c010 = lut3d->lut[PREV(s->r)][NEXT(s->g)][PREV(s->b)];
    const struct rgbvec c011 = lut3d->lut[PREV(s->r)][NEXT(s->g)][NEXT(s->b)];
    const struct rgbvec c100 = lut3d->lut[NEXT(s->r)][PREV(s->g)][PREV(s->b)];
    const struct rgbvec c101 = lut3d->lut[NEXT(s->r)][PREV(s->g)][NEXT(s->b)];
    const struct rgbvec c110 = lut3d->lut[NEXT(s->r)][NEXT(s->g)][PREV(s->b)];
    const struct rgbvec c111 = lut3d->lut[NEXT(s->r)][NEXT(s->g)][NEXT(s->b)];
    const struct rgbvec c00  = lerp(&c000, &c100, d.r);
    const struct rgbvec c10  = lerp(&c010, &c110, d.r);
    const struct rgbvec c01  = lerp(&c001, &c101, d.r);
    const struct rgbvec c11  = lerp(&c011, &c111, d.r);
    const struct rgbvec c0   = lerp(&c00,  &c10,  d.g);
    const struct rgbvec c1   = lerp(&c01,  &c11,  d.g);
    const struct rgbvec c    = lerp(&c0,   &c1,   d.b);
    return c;
}
