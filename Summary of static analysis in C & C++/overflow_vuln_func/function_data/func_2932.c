target_ulong helper_mul_suov(CPUTriCoreState *env, target_ulong r1,
                             target_ulong r2)
{
    int64_t t1 = extract64(r1, 0, 32);
    int64_t t2 = extract64(r2, 0, 32);
    int64_t result = t1 * t2;

    return suov32(env, result);
}
