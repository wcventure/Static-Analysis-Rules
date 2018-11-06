void do_load_fpscr (void)
{
    /
    union {
        float64 d;
        struct {
            uint32_t u[2];
        } s;
    } u;
    int i;

#ifdef WORDS_BIGENDIAN
#define WORD0 0
#define WORD1 1
#else
#define WORD0 1
#define WORD1 0
#endif
    u.s.u[WORD0] = 0;
    u.s.u[WORD1] = 0;
    for (i = 0; i < 8; i++)
        u.s.u[WORD1] |= env->fpscr[i] << (4 * i);
    FT0 = u.d;
}
