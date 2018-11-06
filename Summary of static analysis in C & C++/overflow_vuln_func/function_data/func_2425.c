void dump_exec_info(FILE *f, fprintf_function cpu_fprintf)
{
    int i, target_code_size, max_target_code_size;
    int direct_jmp_count, direct_jmp2_count, cross_page;
    TranslationBlock *tb;

    target_code_size = 0;
    max_target_code_size = 0;
    cross_page = 0;
    direct_jmp_count = 0;
    direct_jmp2_count = 0;
    for (i = 0; i < tcg_ctx.tb_ctx.nb_tbs; i++) {
        tb = &tcg_ctx.tb_ctx.tbs[i];
        target_code_size += tb->size;
        if (tb->size > max_target_code_size) {
            max_target_code_size = tb->size;
        }
        if (tb->page_addr[1] != -1) {
            cross_page++;
        }
        if (tb->tb_next_offset[0] != 0xffff) {
            direct_jmp_count++;
            if (tb->tb_next_offset[1] != 0xffff) {
                direct_jmp2_count++;
            }
        }
    }
    /
    cpu_fprintf(f, "Translation buffer state:\n");
    cpu_fprintf(f, "gen code size       %td/%zd\n",
                tcg_ctx.code_gen_ptr - tcg_ctx.code_gen_buffer,
                tcg_ctx.code_gen_buffer_max_size);
    cpu_fprintf(f, "TB count            %d/%d\n",
            tcg_ctx.tb_ctx.nb_tbs, tcg_ctx.code_gen_max_blocks);
    cpu_fprintf(f, "TB avg target size  %d max=%d bytes\n",
            tcg_ctx.tb_ctx.nb_tbs ? target_code_size /
                    tcg_ctx.tb_ctx.nb_tbs : 0,
            max_target_code_size);
    cpu_fprintf(f, "TB avg host size    %td bytes (expansion ratio: %0.1f)\n",
            tcg_ctx.tb_ctx.nb_tbs ? (tcg_ctx.code_gen_ptr -
                                     tcg_ctx.code_gen_buffer) /
                                     tcg_ctx.tb_ctx.nb_tbs : 0,
                target_code_size ? (double) (tcg_ctx.code_gen_ptr -
                                             tcg_ctx.code_gen_buffer) /
                                             target_code_size : 0);
    cpu_fprintf(f, "cross page TB count %d (%d%%)\n", cross_page,
            tcg_ctx.tb_ctx.nb_tbs ? (cross_page * 100) /
                                    tcg_ctx.tb_ctx.nb_tbs : 0);
    cpu_fprintf(f, "direct jump count   %d (%d%%) (2 jumps=%d %d%%)\n",
                direct_jmp_count,
                tcg_ctx.tb_ctx.nb_tbs ? (direct_jmp_count * 100) /
                        tcg_ctx.tb_ctx.nb_tbs : 0,
                direct_jmp2_count,
                tcg_ctx.tb_ctx.nb_tbs ? (direct_jmp2_count * 100) /
                        tcg_ctx.tb_ctx.nb_tbs : 0);
    cpu_fprintf(f, "\nStatistics:\n");
    cpu_fprintf(f, "TB flush count      %d\n", tcg_ctx.tb_ctx.tb_flush_count);
    cpu_fprintf(f, "TB invalidate count %d\n",
            tcg_ctx.tb_ctx.tb_phys_invalidate_count);
    cpu_fprintf(f, "TLB flush count     %d\n", tlb_flush_count);
    tcg_dump_info(f, cpu_fprintf);
}
