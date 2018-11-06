static inline void *alloc_code_gen_buffer(void)
{
    void *buf = static_code_gen_buffer;
#ifdef __mips__
    if (cross_256mb(buf, tcg_ctx.code_gen_buffer_size)) {
        buf = split_cross_256mb(buf, tcg_ctx.code_gen_buffer_size);
    }
#endif
    map_exec(buf, tcg_ctx.code_gen_buffer_size);
    return buf;
}
#elif defined(USE_MMAP)
static inline void *alloc_code_gen_buffer(void)
{
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    uintptr_t start = 0;
    void *buf;

    /
# if defined(__PIE__) || defined(__PIC__)
    /
# elif defined(__x86_64__) && defined(MAP_32BIT)
    /
    flags |= MAP_32BIT;
    /
    if (tcg_ctx.code_gen_buffer_size > 800u * 1024 * 1024) {
        tcg_ctx.code_gen_buffer_size = 800u * 1024 * 1024;
    }
# elif defined(__sparc__)
    start = 0x40000000ul;
# elif defined(__s390x__)
    start = 0x90000000ul;
# elif defined(__mips__)
    /
#  ifdef CONFIG_USER_ONLY
    start = 0x68000000ul;
#  elif _MIPS_SIM == _ABI64
    start = 0x128000000ul;
#  else
    start = 0x08000000ul;
#  endif
# endif

    buf = mmap((void *)start, tcg_ctx.code_gen_buffer_size,
               PROT_WRITE | PROT_READ | PROT_EXEC, flags, -1, 0);
    if (buf == MAP_FAILED) {
        return NULL;
    }

#ifdef __mips__
    if (cross_256mb(buf, tcg_ctx.code_gen_buffer_size)) {
        /
        size_t size2, size1 = tcg_ctx.code_gen_buffer_size;
        void *buf2 = mmap(NULL, size1, PROT_WRITE | PROT_READ | PROT_EXEC,
                          flags, -1, 0);
        if (buf2 != MAP_FAILED) {
            if (!cross_256mb(buf2, size1)) {
                /
                munmap(buf, size1);
                return buf2;
            }
            /
            munmap(buf2, size1);
        }

        /
        buf2 = split_cross_256mb(buf, size1);
        size2 = tcg_ctx.code_gen_buffer_size;
        munmap(buf + (buf == buf2 ? size2 : 0), size1 - size2);
        return buf2;
    }
#endif

    return buf;
}
