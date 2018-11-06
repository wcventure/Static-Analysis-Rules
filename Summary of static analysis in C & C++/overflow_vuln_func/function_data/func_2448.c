static int gdb_get_avr_reg(CPUState *env, uint8_t *mem_buf, int n)
{
    if (n < 32) {
#ifdef WORDS_BIGENDIAN
        stq_p(mem_buf, env->avr[n].u64[0]);
        stq_p(mem_buf+8, env->avr[n].u64[1]);
#else
        stq_p(mem_buf, env->avr[n].u64[1]);
        stq_p(mem_buf+8, env->avr[n].u64[0]);
#endif
        return 16;
    }
    if (n == 33) {
        stl_p(mem_buf, env->vscr);
        return 4;
    }
    if (n == 34) {
        stl_p(mem_buf, (uint32_t)env->spr[SPR_VRSAVE]);
        return 4;
    }
    return 0;
}
