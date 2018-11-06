int ff_get_cpu_flags_x86(void)
{
    int rval = 0;
    int eax, ebx, ecx, edx;
    int max_std_level, max_ext_level, std_caps=0, ext_caps=0;
    int family=0, model=0;
    union { int i[3]; char c[12]; } vendor;

#if ARCH_X86_32
    x86_reg a, c;
    __asm__ volatile (
        /
        /
        "pushfl\n\t"
        "pop %0\n\t"
        "mov %0, %1\n\t"

        /
        /
        "xor $0x200000, %0\n\t"
        "push %0\n\t"
        "popfl\n\t"

        /
        "pushfl\n\t"
        "pop %0\n\t"
        : "=a" (a), "=c" (c)
        :
        : "cc"
        );

    if (a == c)
        return 0; /
#endif

    cpuid(0, max_std_level, vendor.i[0], vendor.i[2], vendor.i[1]);

    if(max_std_level >= 1){
        cpuid(1, eax, ebx, ecx, std_caps);
        family = ((eax>>8)&0xf) + ((eax>>20)&0xff);
        model  = ((eax>>4)&0xf) + ((eax>>12)&0xf0);
        if (std_caps & (1<<23))
            rval |= AV_CPU_FLAG_MMX;
        if (std_caps & (1<<25))
            rval |= AV_CPU_FLAG_MMX2
#if HAVE_SSE
                  | AV_CPU_FLAG_SSE;
        if (std_caps & (1<<26))
            rval |= AV_CPU_FLAG_SSE2;
        if (ecx & 1)
            rval |= AV_CPU_FLAG_SSE3;
        if (ecx & 0x00000200 )
            rval |= AV_CPU_FLAG_SSSE3;
        if (ecx & 0x00080000 )
            rval |= AV_CPU_FLAG_SSE4;
        if (ecx & 0x00100000 )
            rval |= AV_CPU_FLAG_SSE42;
#if HAVE_AVX
        /
        if ((ecx & 0x18000000) == 0x18000000) {
            /
            xgetbv(0, eax, edx);
            if ((eax & 0x6) == 0x6)
                rval |= AV_CPU_FLAG_AVX;
        }
#endif
#endif
                  ;
    }

    cpuid(0x80000000, max_ext_level, ebx, ecx, edx);

    if(max_ext_level >= 0x80000001){
        cpuid(0x80000001, eax, ebx, ecx, ext_caps);
        if (ext_caps & (1<<31))
            rval |= AV_CPU_FLAG_3DNOW;
        if (ext_caps & (1<<30))
            rval |= AV_CPU_FLAG_3DNOWEXT;
        if (ext_caps & (1<<23))
            rval |= AV_CPU_FLAG_MMX;
        if (ext_caps & (1<<22))
            rval |= AV_CPU_FLAG_MMX2;

        /
        if (!strncmp(vendor.c, "AuthenticAMD", 12) &&
            rval & AV_CPU_FLAG_SSE2 && !(ecx & 0x00000040)) {
            rval |= AV_CPU_FLAG_SSE2SLOW;
        }
    }

    if (!strncmp(vendor.c, "GenuineIntel", 12)) {
        if (family == 6 && (model == 9 || model == 13 || model == 14)) {
            /
            if (rval & AV_CPU_FLAG_SSE2) rval ^= AV_CPU_FLAG_SSE2SLOW|AV_CPU_FLAG_SSE2;
            if (rval & AV_CPU_FLAG_SSE3) rval ^= AV_CPU_FLAG_SSE3SLOW|AV_CPU_FLAG_SSE3;
        }
        /
        if (family == 6 && model == 28)
            rval |= AV_CPU_FLAG_ATOM;
    }

    return rval;
}
