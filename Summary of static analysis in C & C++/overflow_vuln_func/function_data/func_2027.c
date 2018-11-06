void cpu_loop(CPUMIPSState *env)
{
    CPUState *cs = CPU(mips_env_get_cpu(env));
    target_siginfo_t info;
    int trapnr, ret;
    unsigned int syscall_num;

    for(;;) {
        cpu_exec_start(cs);
        trapnr = cpu_mips_exec(env);
        cpu_exec_end(cs);
        switch(trapnr) {
        case EXCP_SYSCALL:
            syscall_num = env->active_tc.gpr[2] - 4000;
            env->active_tc.PC += 4;
            if (syscall_num >= sizeof(mips_syscall_args)) {
                ret = -TARGET_ENOSYS;
            } else {
                int nb_args;
                abi_ulong sp_reg;
                abi_ulong arg5 = 0, arg6 = 0, arg7 = 0, arg8 = 0;

                nb_args = mips_syscall_args[syscall_num];
                sp_reg = env->active_tc.gpr[29];
                switch (nb_args) {
                /
                case 8:
                    if ((ret = get_user_ual(arg8, sp_reg + 28)) != 0) {
                        goto done_syscall;
                    }
                case 7:
                    if ((ret = get_user_ual(arg7, sp_reg + 24)) != 0) {
                        goto done_syscall;
                    }
                case 6:
                    if ((ret = get_user_ual(arg6, sp_reg + 20)) != 0) {
                        goto done_syscall;
                    }
                case 5:
                    if ((ret = get_user_ual(arg5, sp_reg + 16)) != 0) {
                        goto done_syscall;
                    }
                default:
                    break;
                }
                ret = do_syscall(env, env->active_tc.gpr[2],
                                 env->active_tc.gpr[4],
                                 env->active_tc.gpr[5],
                                 env->active_tc.gpr[6],
                                 env->active_tc.gpr[7],
                                 arg5, arg6, arg7, arg8);
            }
done_syscall:
            if (ret == -TARGET_QEMU_ESIGRETURN) {
                /
                break;
            }
            if ((unsigned int)ret >= (unsigned int)(-1133)) {
                env->active_tc.gpr[7] = 1; /
                ret = -ret;
            } else {
                env->active_tc.gpr[7] = 0; /
            }
            env->active_tc.gpr[2] = ret;
            break;
        case EXCP_TLBL:
        case EXCP_TLBS:
        case EXCP_AdEL:
        case EXCP_AdES:
            info.si_signo = TARGET_SIGSEGV;
            info.si_errno = 0;
            /
            info.si_code = TARGET_SEGV_MAPERR;
            info._sifields._sigfault._addr = env->CP0_BadVAddr;
            queue_signal(env, info.si_signo, &info);
            break;
        case EXCP_CpU:
        case EXCP_RI:
            info.si_signo = TARGET_SIGILL;
            info.si_errno = 0;
            info.si_code = 0;
            queue_signal(env, info.si_signo, &info);
            break;
        case EXCP_INTERRUPT:
            /
            break;
        case EXCP_DEBUG:
            {
                int sig;

                sig = gdb_handlesig (env, TARGET_SIGTRAP);
                if (sig)
                  {
                    info.si_signo = sig;
                    info.si_errno = 0;
                    info.si_code = TARGET_TRAP_BRKPT;
                    queue_signal(env, info.si_signo, &info);
                  }
            }
            break;
        case EXCP_SC:
            if (do_store_exclusive(env)) {
                info.si_signo = TARGET_SIGSEGV;
                info.si_errno = 0;
                info.si_code = TARGET_SEGV_MAPERR;
                info._sifields._sigfault._addr = env->active_tc.PC;
                queue_signal(env, info.si_signo, &info);
            }
            break;
        case EXCP_DSPDIS:
            info.si_signo = TARGET_SIGILL;
            info.si_errno = 0;
            info.si_code = TARGET_ILL_ILLOPC;
            queue_signal(env, info.si_signo, &info);
            break;
        default:
            //        error:
            fprintf(stderr, "qemu: unhandled CPU exception 0x%x - aborting\n",
                    trapnr);
            cpu_dump_state(env, stderr, fprintf, 0);
            abort();
        }
        process_pending_signals(env);
    }
}
