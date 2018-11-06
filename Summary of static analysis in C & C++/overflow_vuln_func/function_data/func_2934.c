target_ulong helper_msub32_suov(CPUTriCoreState *env, target_ulong r1,
                                target_ulong r2, target_ulong r3)
{
    int64_t t1 = extract64(r1, 0, 32);
    int64_t t2 = extract64(r2, 0, 32);
    int64_t t3 = extract64(r3, 0, 32);
    int64_t result;

    result = t2 - (t1 * t3);
    return suov32(env, result);
}
