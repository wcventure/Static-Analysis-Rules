void HELPER(cksm)(uint32_t r1, uint32_t r2)
{
    uint64_t src = get_address_31fix(r2);
    uint64_t src_len = env->regs[(r2 + 1) & 15];
    uint64_t cksm = 0;

    while (src_len >= 4) {
        cksm += ldl(src);
        cksm = cksm_overflow(cksm);

        /
        src_len -= 4;
        src += 4;
    }

    switch (src_len) {
    case 0:
        break;
    case 1:
        cksm += ldub(src);
        cksm = cksm_overflow(cksm);
        break;
    case 2:
        cksm += lduw(src);
        cksm = cksm_overflow(cksm);
        break;
    case 3:
        /
        cksm += lduw(src) << 8;
        cksm += ldub(src + 2);
        cksm = cksm_overflow(cksm);
        break;
    }

    /
    env->regs[(r2 + 1) & 15] = 0;

    /
    env->regs[r1] = (env->regs[r1] & 0xffffffff00000000ULL) | (uint32_t)cksm;
}
