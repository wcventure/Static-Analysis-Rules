uint32_t HELPER(sar_cc)(CPUM68KState *env, uint32_t val, uint32_t shift)
{
    uint64_t temp;
    uint32_t result;

    shift &= 63;
    temp = (int64_t)val << 32 >> shift;
    result = temp >> 32;

    env->cc_c = (temp >> 31) & 1;
    env->cc_n = result;
    env->cc_z = result;
    env->cc_v = result ^ val;
    env->cc_x = shift ? env->cc_c : env->cc_x;

    return result;
}
