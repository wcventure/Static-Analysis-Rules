static int skip_check(MpegEncContext *s, Picture *p, Picture *ref)
{
    int x, y, plane;
    int score = 0;
    int64_t score64 = 0;

    for (plane = 0; plane < 3; plane++) {
        const int stride = p->f.linesize[plane];
        const int bw = plane ? 1 : 2;
        for (y = 0; y < s->mb_height * bw; y++) {
            for (x = 0; x < s->mb_width * bw; x++) {
                int off = p->shared ? 0 : 16;
                uint8_t *dptr = p->f.data[plane] + 8 * (x + y * stride) + off;
                uint8_t *rptr = ref->f.data[plane] + 8 * (x + y * stride);
                int v   = s->dsp.frame_skip_cmp[1](s, dptr, rptr, stride, 8);

                switch (s->avctx->frame_skip_exp) {
                case 0: score    =  FFMAX(score, v);          break;
                case 1: score   += FFABS(v);                  break;
                case 2: score   += v * v;                     break;
                case 3: score64 += FFABS(v * v * (int64_t)v); break;
                case 4: score64 += v * v * (int64_t)(v * v);  break;
                }
            }
        }
    }

    if (score)
        score64 = score;

    if (score64 < s->avctx->frame_skip_threshold)
        return 1;
    if (score64 < ((s->avctx->frame_skip_factor * (int64_t)s->lambda) >> 8))
        return 1;
    return 0;
}
