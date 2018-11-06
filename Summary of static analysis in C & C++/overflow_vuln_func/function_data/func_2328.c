void do_fctiwz (void)
{
    union {
        double d;
        uint64_t i;
    } p;

    /
    p.i = float64_to_int32_round_to_zero(FT0, &env->fp_status);
    p.i |= 0xFFF80000ULL << 32;
    FT0 = p.d;
}
