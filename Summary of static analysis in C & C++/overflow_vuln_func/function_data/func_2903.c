void cpu_x86_dump_state(CPUX86State *env, FILE *f, int flags)
{
    int eflags, i;
    char cc_op_name[32];
    static const char *seg_name[6] = { "ES", "CS", "SS", "DS", "FS", "GS" };

    eflags = env->eflags;
    fprintf(f, "EAX=%08x EBX=%08x ECX=%08x EDX=%08x\n"
            "ESI=%08x EDI=%08x EBP=%08x ESP=%08x\n"
            "EIP=%08x EFL=%08x [%c%c%c%c%c%c%c]    CPL=%d II=%d A20=%d\n",
            env->regs[R_EAX], env->regs[R_EBX], env->regs[R_ECX], env->regs[R_EDX], 
            env->regs[R_ESI], env->regs[R_EDI], env->regs[R_EBP], env->regs[R_ESP], 
            env->eip, eflags,
            eflags & DF_MASK ? 'D' : '-',
            eflags & CC_O ? 'O' : '-',
            eflags & CC_S ? 'S' : '-',
            eflags & CC_Z ? 'Z' : '-',
            eflags & CC_A ? 'A' : '-',
            eflags & CC_P ? 'P' : '-',
            eflags & CC_C ? 'C' : '-',
            env->hflags & HF_CPL_MASK, 
            (env->hflags >> HF_INHIBIT_IRQ_SHIFT) & 1,
            (env->a20_mask >> 20) & 1);
    for(i = 0; i < 6; i++) {
        SegmentCache *sc = &env->segs[i];
        fprintf(f, "%s =%04x %08x %08x %08x\n",
                seg_name[i],
                sc->selector,
                (int)sc->base,
                sc->limit,
                sc->flags);
    }
    fprintf(f, "LDT=%04x %08x %08x %08x\n",
            env->ldt.selector,
            (int)env->ldt.base,
            env->ldt.limit,
            env->ldt.flags);
    fprintf(f, "TR =%04x %08x %08x %08x\n",
            env->tr.selector,
            (int)env->tr.base,
            env->tr.limit,
            env->tr.flags);
    fprintf(f, "GDT=     %08x %08x\n",
            (int)env->gdt.base, env->gdt.limit);
    fprintf(f, "IDT=     %08x %08x\n",
            (int)env->idt.base, env->idt.limit);
    fprintf(f, "CR0=%08x CR2=%08x CR3=%08x CR4=%08x\n",
            env->cr[0], env->cr[2], env->cr[3], env->cr[4]);
    
    if (flags & X86_DUMP_CCOP) {
        if ((unsigned)env->cc_op < CC_OP_NB)
            strcpy(cc_op_name, cc_op_str[env->cc_op]);
        else
            snprintf(cc_op_name, sizeof(cc_op_name), "[%d]", env->cc_op);
        fprintf(f, "CCS=%08x CCD=%08x CCO=%-8s\n",
                env->cc_src, env->cc_dst, cc_op_name);
    }
    if (flags & X86_DUMP_FPU) {
        fprintf(f, "ST0=%f ST1=%f ST2=%f ST3=%f\n", 
                (double)env->fpregs[0], 
                (double)env->fpregs[1], 
                (double)env->fpregs[2], 
                (double)env->fpregs[3]);
        fprintf(f, "ST4=%f ST5=%f ST6=%f ST7=%f\n", 
                (double)env->fpregs[4], 
                (double)env->fpregs[5], 
                (double)env->fpregs[7], 
                (double)env->fpregs[8]);
    }
}
