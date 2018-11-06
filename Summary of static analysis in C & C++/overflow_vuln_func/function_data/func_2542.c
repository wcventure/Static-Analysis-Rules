do {\
    static void __attribute__((unused)) *__op_label ## n = &&label ## n;\
    goto *(void *)(((TranslationBlock *)tbparam)->tb_next[n]);\
label ## n:\
    T0 = (long)(tbparam) + (n);\
    EIP = eip;\
} while (0)
