static av_always_inline void predict(PredictorState *ps, int *coef,
                                     int output_enable)
{
    const SoftFloat a     = { 1023410176, 0 };  // 61.0 / 64
    const SoftFloat alpha = {  973078528, 0 };  // 29.0 / 32
    SoftFloat e0, e1;
    SoftFloat pv;
    SoftFloat k1, k2;
    SoftFloat   r0 = ps->r0,     r1 = ps->r1;
    SoftFloat cor0 = ps->cor0, cor1 = ps->cor1;
    SoftFloat var0 = ps->var0, var1 = ps->var1;
    SoftFloat tmp;

    if (var0.exp > 1 || (var0.exp == 1 && var0.mant > 0x20000000)) {
        k1 = av_mul_sf(cor0, flt16_even(av_div_sf(a, var0)));
    }
    else {
        k1.mant = 0;
        k1.exp = 0;
    }

    if (var1.exp > 1 || (var1.exp == 1 && var1.mant > 0x20000000)) {
        k2 = av_mul_sf(cor1, flt16_even(av_div_sf(a, var1)));
    }
    else {
        k2.mant = 0;
        k2.exp = 0;
    }

    tmp = av_mul_sf(k1, r0);
    pv = flt16_round(av_add_sf(tmp, av_mul_sf(k2, r1)));
    if (output_enable) {
        int shift = 28 - pv.exp;

        if (shift < 31) {
            if (shift > 0) {
                *coef += (pv.mant + (1 << (shift - 1))) >> shift;
            } else
                *coef += pv.mant << -shift;
        }
    }

    e0 = av_int2sf(*coef, 2);
    e1 = av_sub_sf(e0, tmp);

    ps->cor1 = flt16_trunc(av_add_sf(av_mul_sf(alpha, cor1), av_mul_sf(r1, e1)));
    tmp = av_add_sf(av_mul_sf(r1, r1), av_mul_sf(e1, e1));
    tmp.exp--;
    ps->var1 = flt16_trunc(av_add_sf(av_mul_sf(alpha, var1), tmp));
    ps->cor0 = flt16_trunc(av_add_sf(av_mul_sf(alpha, cor0), av_mul_sf(r0, e0)));
    tmp = av_add_sf(av_mul_sf(r0, r0), av_mul_sf(e0, e0));
    tmp.exp--;
    ps->var0 = flt16_trunc(av_add_sf(av_mul_sf(alpha, var0), tmp));

    ps->r1 = flt16_trunc(av_mul_sf(a, av_sub_sf(r0, av_mul_sf(k1, e0))));
    ps->r0 = flt16_trunc(av_mul_sf(a, e0));
}
