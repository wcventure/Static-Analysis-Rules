uint32_t HELPER(shl_cc)(CPUM68KState *env, uint32_t val, uint32_t shift)
{
    uint64_t result;

    shift &= 63;
    result = (uint64_t)val << shift;

    env->cc_c = (result >> 32) & 1;
    env->cc_n = result;
    env->cc_z = result;
    env->cc_v = 0;
    env->cc_x = shift ? env->cc_c : env->cc_x;

    return result;
}
