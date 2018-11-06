int cpu_gen_code(CPUState *env, TranslationBlock *tb,
                 int max_code_size, int *gen_code_size_ptr)
{
    uint8_t *gen_code_buf;
    int gen_code_size;

    if (gen_intermediate_code(env, tb) < 0)
        return -1;
    
    /
    tb->tb_next_offset[0] = 0xffff;
    tb->tb_next_offset[1] = 0xffff;
    gen_code_buf = tb->tc_ptr;
#ifdef USE_DIRECT_JUMP
    /
    tb->tb_jmp_offset[2] = 0xffff;
    tb->tb_jmp_offset[3] = 0xffff;
#endif
    dyngen_labels(gen_labels, nb_gen_labels, gen_code_buf, gen_opc_buf);
    
    gen_code_size = dyngen_code(gen_code_buf, tb->tb_next_offset,
#ifdef USE_DIRECT_JUMP
                                tb->tb_jmp_offset,
#else
                                NULL,
#endif
                                gen_opc_buf, gen_opparam_buf, gen_labels);
    *gen_code_size_ptr = gen_code_size;
#ifdef DEBUG_DISAS
    if (loglevel & CPU_LOG_TB_OUT_ASM) {
        fprintf(logfile, "OUT: [size=%d]\n", *gen_code_size_ptr);
        disas(logfile, tb->tc_ptr, *gen_code_size_ptr);
        fprintf(logfile, "\n");
        fflush(logfile);
    }
#endif
    return 0;
}
