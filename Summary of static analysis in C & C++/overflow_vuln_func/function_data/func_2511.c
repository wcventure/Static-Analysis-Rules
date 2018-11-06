uint64_t helper_cvttq(CPUAlphaState *env, uint64_t a)
{
    return inline_cvttq(env, a, FP_STATUS.float_rounding_mode, 1);
}
