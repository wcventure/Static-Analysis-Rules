do {\
    static void __attribute__((unused)) *__op_label ## n = &&label ## n;\
    asm volatile ("b %0" : : "i" (&__op_jmp ## n));\
label ## n:\
    T0 = (long)(tbparam) + (n);\
    EIP = eip;\
} while (0)
