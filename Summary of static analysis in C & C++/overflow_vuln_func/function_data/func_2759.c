    return buf;
}
#else
static inline void *alloc_code_gen_buffer(void)
{
    void *buf = g_try_malloc(tcg_ctx.code_gen_buffer_size);

    if (buf == NULL) {
        return NULL;
    }

#ifdef __mips__
    if (cross_256mb(buf, tcg_ctx.code_gen_buffer_size)) {
        void *buf2 = g_malloc(tcg_ctx.code_gen_buffer_size);
        if (buf2 != NULL && !cross_256mb(buf2, size1)) {
            /
            free(buf);
            buf = buf2;
        } else {
            /
            free(buf2);
            buf = split_cross_256mb(buf, tcg_ctx.code_gen_buffer_size);
        }
    }
#endif

    map_exec(buf, tcg_ctx.code_gen_buffer_size);
    return buf;
}
