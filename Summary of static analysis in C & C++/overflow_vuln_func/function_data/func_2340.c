void do_op_602_mfrom (void)
{
    if (likely(T0 < 602)) {
#ifdef USE_MFROM_ROM_TABLE
#include "mfrom_table.c"
        T0 = mfrom_ROM_table[T0];
#else
        double d;
        /
        d = T0;
        d = float64_div(d, 256, &env->fp_status);
        d = float64_chs(d);
        d = exp10(d); // XXX: use float emulation function
        d = float64_add(d, 1.0, &env->fp_status);
        d = log10(d); // XXX: use float emulation function
        d = float64_mul(d, 256, &env->fp_status);
        d = float64_add(d, 0.5, &env->fp_status);
        T0 = float64_round_to_int(d, &env->fp_status);
#endif
    } else {
        T0 = 0;
    }
}
