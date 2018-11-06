void cpu_dump_state(CPUState *env, FILE *f, 
                    int (*cpu_fprintf)(FILE *f, const char *fmt, ...),
                    int flags)
{
#if defined(TARGET_PPC64) || 1
#define FILL ""
#define RGPL  4
#define RFPL  4
#else
#define FILL "        "
#define RGPL  8
#define RFPL  4
#endif

    int i;

    cpu_fprintf(f, "NIP " REGX " LR " REGX " CTR " REGX "\n",
                env->nip, env->lr, env->ctr);
    cpu_fprintf(f, "MSR " REGX FILL " XER %08x      TB %08x %08x "
#if !defined(CONFIG_USER_ONLY)
                "DECR %08x"
#endif
                "\n",
                do_load_msr(env), load_xer(env), cpu_ppc_load_tbu(env),
                cpu_ppc_load_tbl(env)
#if !defined(CONFIG_USER_ONLY)
                , cpu_ppc_load_decr(env)
#endif
                );
    for (i = 0; i < 32; i++) {
        if ((i & (RGPL - 1)) == 0)
            cpu_fprintf(f, "GPR%02d", i);
        cpu_fprintf(f, " " REGX, env->gpr[i]);
        if ((i & (RGPL - 1)) == (RGPL - 1))
            cpu_fprintf(f, "\n");
    }
    cpu_fprintf(f, "CR ");
    for (i = 0; i < 8; i++)
        cpu_fprintf(f, "%01x", env->crf[i]);
    cpu_fprintf(f, "  [");
    for (i = 0; i < 8; i++) {
        char a = '-';
        if (env->crf[i] & 0x08)
            a = 'L';
        else if (env->crf[i] & 0x04)
            a = 'G';
        else if (env->crf[i] & 0x02)
            a = 'E';
        cpu_fprintf(f, " %c%c", a, env->crf[i] & 0x01 ? 'O' : ' ');
    }
    cpu_fprintf(f, " ]             " FILL "RES " REGX "\n", env->reserve);
    for (i = 0; i < 32; i++) {
        if ((i & (RFPL - 1)) == 0)
            cpu_fprintf(f, "FPR%02d", i);
        cpu_fprintf(f, " %016" PRIx64, *((uint64_t *)&env->fpr[i]));
        if ((i & (RFPL - 1)) == (RFPL - 1))
            cpu_fprintf(f, "\n");
    }
    cpu_fprintf(f, "SRR0 " REGX " SRR1 " REGX "         " FILL FILL FILL
                "SDR1 " REGX "\n",
                env->spr[SPR_SRR0], env->spr[SPR_SRR1], env->sdr1);

#undef REGX
#undef RGPL
#undef RFPL
#undef FILL
}
