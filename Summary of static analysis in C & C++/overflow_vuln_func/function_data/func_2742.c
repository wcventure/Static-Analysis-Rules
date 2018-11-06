void HELPER(divs)(CPUM68KState *env, uint32_t word)
{
    int32_t num;
    int32_t den;
    int32_t quot;
    int32_t rem;

    num = env->div1;
    den = env->div2;
    if (den == 0) {
        raise_exception(env, EXCP_DIV0);
    }
    quot = num / den;
    rem = num % den;

    env->cc_v = (word && quot != (int16_t)quot ? -1 : 0);
    env->cc_z = quot;
    env->cc_n = quot;
    env->cc_c = 0;

    env->div1 = quot;
    env->div2 = rem;
}
