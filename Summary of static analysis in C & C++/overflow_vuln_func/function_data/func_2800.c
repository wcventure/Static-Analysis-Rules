abi_long do_syscall(void *cpu_env, int num, abi_long arg1,
                    abi_long arg2, abi_long arg3, abi_long arg4,
                    abi_long arg5, abi_long arg6)
{
    abi_long ret;
    struct stat st;
    struct statfs stfs;
    void *p;

#ifdef DEBUG
    gemu_log("syscall %d", num);
#endif
    if(do_strace)
        print_syscall(num, arg1, arg2, arg3, arg4, arg5, arg6);

    switch(num) {
    case TARGET_NR_exit:
#ifdef CONFIG_USE_NPTL
      /
      /
      if (first_cpu->next_cpu) {
          TaskState *ts;
          CPUState **lastp;
          CPUState *p;

          cpu_list_lock();
          lastp = &first_cpu;
          p = first_cpu;
          while (p && p != (CPUState *)cpu_env) {
              lastp = &p->next_cpu;
              p = p->next_cpu;
          }
          /
          if (!p)
              abort();
          /
          *lastp = p->next_cpu;
          cpu_list_unlock();
          ts = ((CPUState *)cpu_env)->opaque;
          if (ts->child_tidptr) {
              put_user_u32(0, ts->child_tidptr);
              sys_futex(g2h(ts->child_tidptr), FUTEX_WAKE, INT_MAX,
                        NULL, NULL, 0);
          }
          thread_env = NULL;
          qemu_free(cpu_env);
          qemu_free(ts);
          pthread_exit(NULL);
      }
#endif
#ifdef TARGET_GPROF
        _mcleanup();
#endif
        gdb_exit(cpu_env, arg1);
        _exit(arg1);
        ret = 0; /
        break;
    case TARGET_NR_read:
        if (arg3 == 0)
            ret = 0;
        else {
            if (!(p = lock_user(VERIFY_WRITE, arg2, arg3, 0)))
                goto efault;
            ret = get_errno(read(arg1, p, arg3));
            unlock_user(p, arg2, ret);
        }
        break;
    case TARGET_NR_write:
        if (!(p = lock_user(VERIFY_READ, arg2, arg3, 1)))
            goto efault;
        ret = get_errno(write(arg1, p, arg3));
        unlock_user(p, arg2, 0);
        break;
    case TARGET_NR_open:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(open(path(p),
                             target_to_host_bitmask(arg2, fcntl_flags_tbl),
                             arg3));
        unlock_user(p, arg1, 0);
        break;
#if defined(TARGET_NR_openat) && defined(__NR_openat)
    case TARGET_NR_openat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_openat(arg1,
                                   path(p),
                                   target_to_host_bitmask(arg3, fcntl_flags_tbl),
                                   arg4));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_close:
        ret = get_errno(close(arg1));
        break;
    case TARGET_NR_brk:
        ret = do_brk(arg1);
        break;
    case TARGET_NR_fork:
        ret = get_errno(do_fork(cpu_env, SIGCHLD, 0, 0, 0, 0));
        break;
#ifdef TARGET_NR_waitpid
    case TARGET_NR_waitpid:
        {
            int status;
            ret = get_errno(waitpid(arg1, &status, arg3));
            if (!is_error(ret) && arg2
                && put_user_s32(host_to_target_waitstatus(status), arg2))
                goto efault;
        }
        break;
#endif
#ifdef TARGET_NR_waitid
    case TARGET_NR_waitid:
        {
            siginfo_t info;
            info.si_pid = 0;
            ret = get_errno(waitid(arg1, arg2, &info, arg4));
            if (!is_error(ret) && arg3 && info.si_pid != 0) {
                if (!(p = lock_user(VERIFY_WRITE, arg3, sizeof(target_siginfo_t), 0)))
                    goto efault;
                host_to_target_siginfo(p, &info);
                unlock_user(p, arg3, sizeof(target_siginfo_t));
            }
        }
        break;
#endif
#ifdef TARGET_NR_creat /
    case TARGET_NR_creat:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(creat(p, arg2));
        unlock_user(p, arg1, 0);
        break;
#endif
    case TARGET_NR_link:
        {
            void * p2;
            p = lock_user_string(arg1);
            p2 = lock_user_string(arg2);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(link(p, p2));
            unlock_user(p2, arg2, 0);
            unlock_user(p, arg1, 0);
        }
        break;
#if defined(TARGET_NR_linkat) && defined(__NR_linkat)
    case TARGET_NR_linkat:
        {
            void * p2 = NULL;
            if (!arg2 || !arg4)
                goto efault;
            p  = lock_user_string(arg2);
            p2 = lock_user_string(arg4);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(sys_linkat(arg1, p, arg3, p2, arg5));
            unlock_user(p, arg2, 0);
            unlock_user(p2, arg4, 0);
        }
        break;
#endif
    case TARGET_NR_unlink:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(unlink(p));
        unlock_user(p, arg1, 0);
        break;
#if defined(TARGET_NR_unlinkat) && defined(__NR_unlinkat)
    case TARGET_NR_unlinkat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_unlinkat(arg1, p, arg3));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_execve:
        {
            char **argp, **envp;
            int argc, envc;
            abi_ulong gp;
            abi_ulong guest_argp;
            abi_ulong guest_envp;
            abi_ulong addr;
            char **q;

            argc = 0;
            guest_argp = arg2;
            for (gp = guest_argp; gp; gp += sizeof(abi_ulong)) {
                if (get_user_ual(addr, gp))
                    goto efault;
                if (!addr)
                    break;
                argc++;
            }
            envc = 0;
            guest_envp = arg3;
            for (gp = guest_envp; gp; gp += sizeof(abi_ulong)) {
                if (get_user_ual(addr, gp))
                    goto efault;
                if (!addr)
                    break;
                envc++;
            }

            argp = alloca((argc + 1) * sizeof(void *));
            envp = alloca((envc + 1) * sizeof(void *));

            for (gp = guest_argp, q = argp; gp;
                  gp += sizeof(abi_ulong), q++) {
                if (get_user_ual(addr, gp))
                    goto execve_efault;
                if (!addr)
                    break;
                if (!(*q = lock_user_string(addr)))
                    goto execve_efault;
            }
            *q = NULL;

            for (gp = guest_envp, q = envp; gp;
                  gp += sizeof(abi_ulong), q++) {
                if (get_user_ual(addr, gp))
                    goto execve_efault;
                if (!addr)
                    break;
                if (!(*q = lock_user_string(addr)))
                    goto execve_efault;
            }
            *q = NULL;

            if (!(p = lock_user_string(arg1)))
                goto execve_efault;
            ret = get_errno(execve(p, argp, envp));
            unlock_user(p, arg1, 0);

            goto execve_end;

        execve_efault:
            ret = -TARGET_EFAULT;

        execve_end:
            for (gp = guest_argp, q = argp; *q;
                  gp += sizeof(abi_ulong), q++) {
                if (get_user_ual(addr, gp)
                    || !addr)
                    break;
                unlock_user(*q, addr, 0);
            }
            for (gp = guest_envp, q = envp; *q;
                  gp += sizeof(abi_ulong), q++) {
                if (get_user_ual(addr, gp)
                    || !addr)
                    break;
                unlock_user(*q, addr, 0);
            }
        }
        break;
    case TARGET_NR_chdir:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(chdir(p));
        unlock_user(p, arg1, 0);
        break;
#ifdef TARGET_NR_time
    case TARGET_NR_time:
        {
            time_t host_time;
            ret = get_errno(time(&host_time));
            if (!is_error(ret)
                && arg1
                && put_user_sal(host_time, arg1))
                goto efault;
        }
        break;
#endif
    case TARGET_NR_mknod:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(mknod(p, arg2, arg3));
        unlock_user(p, arg1, 0);
        break;
#if defined(TARGET_NR_mknodat) && defined(__NR_mknodat)
    case TARGET_NR_mknodat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_mknodat(arg1, p, arg3, arg4));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_chmod:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(chmod(p, arg2));
        unlock_user(p, arg1, 0);
        break;
#ifdef TARGET_NR_break
    case TARGET_NR_break:
        goto unimplemented;
#endif
#ifdef TARGET_NR_oldstat
    case TARGET_NR_oldstat:
        goto unimplemented;
#endif
    case TARGET_NR_lseek:
        ret = get_errno(lseek(arg1, arg2, arg3));
        break;
#if defined(TARGET_NR_getxpid) && defined(TARGET_ALPHA)
    /
    case TARGET_NR_getxpid:
        ((CPUAlphaState *)cpu_env)->ir[IR_A4] = getppid();
        ret = get_errno(getpid());
        break;
#endif
#ifdef TARGET_NR_getpid
    case TARGET_NR_getpid:
        ret = get_errno(getpid());
        break;
#endif
    case TARGET_NR_mount:
		{
			/
			void *p2, *p3;
			p = lock_user_string(arg1);
			p2 = lock_user_string(arg2);
			p3 = lock_user_string(arg3);
                        if (!p || !p2 || !p3)
                            ret = -TARGET_EFAULT;
                        else {
                            /
                            if ( ! arg5 )
                                ret = get_errno(mount(p, p2, p3, (unsigned long)arg4, NULL));
                            else
                                ret = get_errno(mount(p, p2, p3, (unsigned long)arg4, g2h(arg5)));
                        }
                        unlock_user(p, arg1, 0);
                        unlock_user(p2, arg2, 0);
                        unlock_user(p3, arg3, 0);
			break;
		}
#ifdef TARGET_NR_umount
    case TARGET_NR_umount:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(umount(p));
        unlock_user(p, arg1, 0);
        break;
#endif
#ifdef TARGET_NR_stime /
    case TARGET_NR_stime:
        {
            time_t host_time;
            if (get_user_sal(host_time, arg1))
                goto efault;
            ret = get_errno(stime(&host_time));
        }
        break;
#endif
    case TARGET_NR_ptrace:
        goto unimplemented;
#ifdef TARGET_NR_alarm /
    case TARGET_NR_alarm:
        ret = alarm(arg1);
        break;
#endif
#ifdef TARGET_NR_oldfstat
    case TARGET_NR_oldfstat:
        goto unimplemented;
#endif
#ifdef TARGET_NR_pause /
    case TARGET_NR_pause:
        ret = get_errno(pause());
        break;
#endif
#ifdef TARGET_NR_utime
    case TARGET_NR_utime:
        {
            struct utimbuf tbuf, *host_tbuf;
            struct target_utimbuf *target_tbuf;
            if (arg2) {
                if (!lock_user_struct(VERIFY_READ, target_tbuf, arg2, 1))
                    goto efault;
                tbuf.actime = tswapl(target_tbuf->actime);
                tbuf.modtime = tswapl(target_tbuf->modtime);
                unlock_user_struct(target_tbuf, arg2, 0);
                host_tbuf = &tbuf;
            } else {
                host_tbuf = NULL;
            }
            if (!(p = lock_user_string(arg1)))
                goto efault;
            ret = get_errno(utime(p, host_tbuf));
            unlock_user(p, arg1, 0);
        }
        break;
#endif
    case TARGET_NR_utimes:
        {
            struct timeval *tvp, tv[2];
            if (arg2) {
                if (copy_from_user_timeval(&tv[0], arg2)
                    || copy_from_user_timeval(&tv[1],
                                              arg2 + sizeof(struct target_timeval)))
                    goto efault;
                tvp = tv;
            } else {
                tvp = NULL;
            }
            if (!(p = lock_user_string(arg1)))
                goto efault;
            ret = get_errno(utimes(p, tvp));
            unlock_user(p, arg1, 0);
        }
        break;
#if defined(TARGET_NR_futimesat) && defined(__NR_futimesat)
    case TARGET_NR_futimesat:
        {
            struct timeval *tvp, tv[2];
            if (arg3) {
                if (copy_from_user_timeval(&tv[0], arg3)
                    || copy_from_user_timeval(&tv[1],
                                              arg3 + sizeof(struct target_timeval)))
                    goto efault;
                tvp = tv;
            } else {
                tvp = NULL;
            }
            if (!(p = lock_user_string(arg2)))
                goto efault;
            ret = get_errno(sys_futimesat(arg1, path(p), tvp));
            unlock_user(p, arg2, 0);
        }
        break;
#endif
#ifdef TARGET_NR_stty
    case TARGET_NR_stty:
        goto unimplemented;
#endif
#ifdef TARGET_NR_gtty
    case TARGET_NR_gtty:
        goto unimplemented;
#endif
    case TARGET_NR_access:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(access(path(p), arg2));
        unlock_user(p, arg1, 0);
        break;
#if defined(TARGET_NR_faccessat) && defined(__NR_faccessat)
    case TARGET_NR_faccessat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_faccessat(arg1, p, arg3));
        unlock_user(p, arg2, 0);
        break;
#endif
#ifdef TARGET_NR_nice /
    case TARGET_NR_nice:
        ret = get_errno(nice(arg1));
        break;
#endif
#ifdef TARGET_NR_ftime
    case TARGET_NR_ftime:
        goto unimplemented;
#endif
    case TARGET_NR_sync:
        sync();
        ret = 0;
        break;
    case TARGET_NR_kill:
        ret = get_errno(kill(arg1, target_to_host_signal(arg2)));
        break;
    case TARGET_NR_rename:
        {
            void *p2;
            p = lock_user_string(arg1);
            p2 = lock_user_string(arg2);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(rename(p, p2));
            unlock_user(p2, arg2, 0);
            unlock_user(p, arg1, 0);
        }
        break;
#if defined(TARGET_NR_renameat) && defined(__NR_renameat)
    case TARGET_NR_renameat:
        {
            void *p2;
            p  = lock_user_string(arg2);
            p2 = lock_user_string(arg4);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(sys_renameat(arg1, p, arg3, p2));
            unlock_user(p2, arg4, 0);
            unlock_user(p, arg2, 0);
        }
        break;
#endif
    case TARGET_NR_mkdir:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(mkdir(p, arg2));
        unlock_user(p, arg1, 0);
        break;
#if defined(TARGET_NR_mkdirat) && defined(__NR_mkdirat)
    case TARGET_NR_mkdirat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_mkdirat(arg1, p, arg3));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_rmdir:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(rmdir(p));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_dup:
        ret = get_errno(dup(arg1));
        break;
    case TARGET_NR_pipe:
        ret = do_pipe(cpu_env, arg1, 0, 0);
        break;
#ifdef TARGET_NR_pipe2
    case TARGET_NR_pipe2:
        ret = do_pipe(cpu_env, arg1, arg2, 1);
        break;
#endif
    case TARGET_NR_times:
        {
            struct target_tms *tmsp;
            struct tms tms;
            ret = get_errno(times(&tms));
            if (arg1) {
                tmsp = lock_user(VERIFY_WRITE, arg1, sizeof(struct target_tms), 0);
                if (!tmsp)
                    goto efault;
                tmsp->tms_utime = tswapl(host_to_target_clock_t(tms.tms_utime));
                tmsp->tms_stime = tswapl(host_to_target_clock_t(tms.tms_stime));
                tmsp->tms_cutime = tswapl(host_to_target_clock_t(tms.tms_cutime));
                tmsp->tms_cstime = tswapl(host_to_target_clock_t(tms.tms_cstime));
            }
            if (!is_error(ret))
                ret = host_to_target_clock_t(ret);
        }
        break;
#ifdef TARGET_NR_prof
    case TARGET_NR_prof:
        goto unimplemented;
#endif
#ifdef TARGET_NR_signal
    case TARGET_NR_signal:
        goto unimplemented;
#endif
    case TARGET_NR_acct:
        if (arg1 == 0) {
            ret = get_errno(acct(NULL));
        } else {
            if (!(p = lock_user_string(arg1)))
                goto efault;
            ret = get_errno(acct(path(p)));
            unlock_user(p, arg1, 0);
        }
        break;
#ifdef TARGET_NR_umount2 /
    case TARGET_NR_umount2:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(umount2(p, arg2));
        unlock_user(p, arg1, 0);
        break;
#endif
#ifdef TARGET_NR_lock
    case TARGET_NR_lock:
        goto unimplemented;
#endif
    case TARGET_NR_ioctl:
        ret = do_ioctl(arg1, arg2, arg3);
        break;
    case TARGET_NR_fcntl:
        ret = do_fcntl(arg1, arg2, arg3);
        break;
#ifdef TARGET_NR_mpx
    case TARGET_NR_mpx:
        goto unimplemented;
#endif
    case TARGET_NR_setpgid:
        ret = get_errno(setpgid(arg1, arg2));
        break;
#ifdef TARGET_NR_ulimit
    case TARGET_NR_ulimit:
        goto unimplemented;
#endif
#ifdef TARGET_NR_oldolduname
    case TARGET_NR_oldolduname:
        goto unimplemented;
#endif
    case TARGET_NR_umask:
        ret = get_errno(umask(arg1));
        break;
    case TARGET_NR_chroot:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(chroot(p));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_ustat:
        goto unimplemented;
    case TARGET_NR_dup2:
        ret = get_errno(dup2(arg1, arg2));
        break;
#if defined(CONFIG_DUP3) && defined(TARGET_NR_dup3)
    case TARGET_NR_dup3:
        ret = get_errno(dup3(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_getppid /
    case TARGET_NR_getppid:
        ret = get_errno(getppid());
        break;
#endif
    case TARGET_NR_getpgrp:
        ret = get_errno(getpgrp());
        break;
    case TARGET_NR_setsid:
        ret = get_errno(setsid());
        break;
#ifdef TARGET_NR_sigaction
    case TARGET_NR_sigaction:
        {
#if defined(TARGET_ALPHA)
            struct target_sigaction act, oact, *pact = 0;
            struct target_old_sigaction *old_act;
            if (arg2) {
                if (!lock_user_struct(VERIFY_READ, old_act, arg2, 1))
                    goto efault;
                act._sa_handler = old_act->_sa_handler;
                target_siginitset(&act.sa_mask, old_act->sa_mask);
                act.sa_flags = old_act->sa_flags;
                act.sa_restorer = 0;
                unlock_user_struct(old_act, arg2, 0);
                pact = &act;
            }
            ret = get_errno(do_sigaction(arg1, pact, &oact));
            if (!is_error(ret) && arg3) {
                if (!lock_user_struct(VERIFY_WRITE, old_act, arg3, 0))
                    goto efault;
                old_act->_sa_handler = oact._sa_handler;
                old_act->sa_mask = oact.sa_mask.sig[0];
                old_act->sa_flags = oact.sa_flags;
                unlock_user_struct(old_act, arg3, 1);
            }
#elif defined(TARGET_MIPS)
	    struct target_sigaction act, oact, *pact, *old_act;

	    if (arg2) {
                if (!lock_user_struct(VERIFY_READ, old_act, arg2, 1))
                    goto efault;
		act._sa_handler = old_act->_sa_handler;
		target_siginitset(&act.sa_mask, old_act->sa_mask.sig[0]);
		act.sa_flags = old_act->sa_flags;
		unlock_user_struct(old_act, arg2, 0);
		pact = &act;
	    } else {
		pact = NULL;
	    }

	    ret = get_errno(do_sigaction(arg1, pact, &oact));

	    if (!is_error(ret) && arg3) {
                if (!lock_user_struct(VERIFY_WRITE, old_act, arg3, 0))
                    goto efault;
		old_act->_sa_handler = oact._sa_handler;
		old_act->sa_flags = oact.sa_flags;
		old_act->sa_mask.sig[0] = oact.sa_mask.sig[0];
		old_act->sa_mask.sig[1] = 0;
		old_act->sa_mask.sig[2] = 0;
		old_act->sa_mask.sig[3] = 0;
		unlock_user_struct(old_act, arg3, 1);
	    }
#else
            struct target_old_sigaction *old_act;
            struct target_sigaction act, oact, *pact;
            if (arg2) {
                if (!lock_user_struct(VERIFY_READ, old_act, arg2, 1))
                    goto efault;
                act._sa_handler = old_act->_sa_handler;
                target_siginitset(&act.sa_mask, old_act->sa_mask);
                act.sa_flags = old_act->sa_flags;
                act.sa_restorer = old_act->sa_restorer;
                unlock_user_struct(old_act, arg2, 0);
                pact = &act;
            } else {
                pact = NULL;
            }
            ret = get_errno(do_sigaction(arg1, pact, &oact));
            if (!is_error(ret) && arg3) {
                if (!lock_user_struct(VERIFY_WRITE, old_act, arg3, 0))
                    goto efault;
                old_act->_sa_handler = oact._sa_handler;
                old_act->sa_mask = oact.sa_mask.sig[0];
                old_act->sa_flags = oact.sa_flags;
                old_act->sa_restorer = oact.sa_restorer;
                unlock_user_struct(old_act, arg3, 1);
            }
#endif
        }
        break;
#endif
    case TARGET_NR_rt_sigaction:
        {
#if defined(TARGET_ALPHA)
            struct target_sigaction act, oact, *pact = 0;
            struct target_rt_sigaction *rt_act;
            /
            if (arg2) {
                if (!lock_user_struct(VERIFY_READ, rt_act, arg2, 1))
                    goto efault;
                act._sa_handler = rt_act->_sa_handler;
                act.sa_mask = rt_act->sa_mask;
                act.sa_flags = rt_act->sa_flags;
                act.sa_restorer = arg5;
                unlock_user_struct(rt_act, arg2, 0);
                pact = &act;
            }
            ret = get_errno(do_sigaction(arg1, pact, &oact));
            if (!is_error(ret) && arg3) {
                if (!lock_user_struct(VERIFY_WRITE, rt_act, arg3, 0))
                    goto efault;
                rt_act->_sa_handler = oact._sa_handler;
                rt_act->sa_mask = oact.sa_mask;
                rt_act->sa_flags = oact.sa_flags;
                unlock_user_struct(rt_act, arg3, 1);
            }
#else
            struct target_sigaction *act;
            struct target_sigaction *oact;

            if (arg2) {
                if (!lock_user_struct(VERIFY_READ, act, arg2, 1))
                    goto efault;
            } else
                act = NULL;
            if (arg3) {
                if (!lock_user_struct(VERIFY_WRITE, oact, arg3, 0)) {
                    ret = -TARGET_EFAULT;
                    goto rt_sigaction_fail;
                }
            } else
                oact = NULL;
            ret = get_errno(do_sigaction(arg1, act, oact));
	rt_sigaction_fail:
            if (act)
                unlock_user_struct(act, arg2, 0);
            if (oact)
                unlock_user_struct(oact, arg3, 1);
#endif
        }
        break;
#ifdef TARGET_NR_sgetmask /
    case TARGET_NR_sgetmask:
        {
            sigset_t cur_set;
            abi_ulong target_set;
            sigprocmask(0, NULL, &cur_set);
            host_to_target_old_sigset(&target_set, &cur_set);
            ret = target_set;
        }
        break;
#endif
#ifdef TARGET_NR_ssetmask /
    case TARGET_NR_ssetmask:
        {
            sigset_t set, oset, cur_set;
            abi_ulong target_set = arg1;
            sigprocmask(0, NULL, &cur_set);
            target_to_host_old_sigset(&set, &target_set);
            sigorset(&set, &set, &cur_set);
            sigprocmask(SIG_SETMASK, &set, &oset);
            host_to_target_old_sigset(&target_set, &oset);
            ret = target_set;
        }
        break;
#endif
#ifdef TARGET_NR_sigprocmask
    case TARGET_NR_sigprocmask:
        {
#if defined(TARGET_ALPHA)
            sigset_t set, oldset;
            abi_ulong mask;
            int how;

            switch (arg1) {
            case TARGET_SIG_BLOCK:
                how = SIG_BLOCK;
                break;
            case TARGET_SIG_UNBLOCK:
                how = SIG_UNBLOCK;
                break;
            case TARGET_SIG_SETMASK:
                how = SIG_SETMASK;
                break;
            default:
                ret = -TARGET_EINVAL;
                goto fail;
            }
            mask = arg2;
            target_to_host_old_sigset(&set, &mask);

            ret = get_errno(sigprocmask(how, &set, &oldset));

            if (!is_error(ret)) {
                host_to_target_old_sigset(&mask, &oldset);
                ret = mask;
                ((CPUAlphaState *)cpu_env)->[IR_V0] = 0; /
            }
#else
            sigset_t set, oldset, *set_ptr;
            int how;

            if (arg2) {
                switch (arg1) {
                case TARGET_SIG_BLOCK:
                    how = SIG_BLOCK;
                    break;
                case TARGET_SIG_UNBLOCK:
                    how = SIG_UNBLOCK;
                    break;
                case TARGET_SIG_SETMASK:
                    how = SIG_SETMASK;
                    break;
                default:
                    ret = -TARGET_EINVAL;
                    goto fail;
                }
                if (!(p = lock_user(VERIFY_READ, arg2, sizeof(target_sigset_t), 1)))
                    goto efault;
                target_to_host_old_sigset(&set, p);
                unlock_user(p, arg2, 0);
                set_ptr = &set;
            } else {
                how = 0;
                set_ptr = NULL;
            }
            ret = get_errno(sigprocmask(how, set_ptr, &oldset));
            if (!is_error(ret) && arg3) {
                if (!(p = lock_user(VERIFY_WRITE, arg3, sizeof(target_sigset_t), 0)))
                    goto efault;
                host_to_target_old_sigset(p, &oldset);
                unlock_user(p, arg3, sizeof(target_sigset_t));
            }
#endif
        }
        break;
#endif
    case TARGET_NR_rt_sigprocmask:
        {
            int how = arg1;
            sigset_t set, oldset, *set_ptr;

            if (arg2) {
                switch(how) {
                case TARGET_SIG_BLOCK:
                    how = SIG_BLOCK;
                    break;
                case TARGET_SIG_UNBLOCK:
                    how = SIG_UNBLOCK;
                    break;
                case TARGET_SIG_SETMASK:
                    how = SIG_SETMASK;
                    break;
                default:
                    ret = -TARGET_EINVAL;
                    goto fail;
                }
                if (!(p = lock_user(VERIFY_READ, arg2, sizeof(target_sigset_t), 1)))
                    goto efault;
                target_to_host_sigset(&set, p);
                unlock_user(p, arg2, 0);
                set_ptr = &set;
            } else {
                how = 0;
                set_ptr = NULL;
            }
            ret = get_errno(sigprocmask(how, set_ptr, &oldset));
            if (!is_error(ret) && arg3) {
                if (!(p = lock_user(VERIFY_WRITE, arg3, sizeof(target_sigset_t), 0)))
                    goto efault;
                host_to_target_sigset(p, &oldset);
                unlock_user(p, arg3, sizeof(target_sigset_t));
            }
        }
        break;
#ifdef TARGET_NR_sigpending
    case TARGET_NR_sigpending:
        {
            sigset_t set;
            ret = get_errno(sigpending(&set));
            if (!is_error(ret)) {
                if (!(p = lock_user(VERIFY_WRITE, arg1, sizeof(target_sigset_t), 0)))
                    goto efault;
                host_to_target_old_sigset(p, &set);
                unlock_user(p, arg1, sizeof(target_sigset_t));
            }
        }
        break;
#endif
    case TARGET_NR_rt_sigpending:
        {
            sigset_t set;
            ret = get_errno(sigpending(&set));
            if (!is_error(ret)) {
                if (!(p = lock_user(VERIFY_WRITE, arg1, sizeof(target_sigset_t), 0)))
                    goto efault;
                host_to_target_sigset(p, &set);
                unlock_user(p, arg1, sizeof(target_sigset_t));
            }
        }
        break;
#ifdef TARGET_NR_sigsuspend
    case TARGET_NR_sigsuspend:
        {
            sigset_t set;
#if defined(TARGET_ALPHA)
            abi_ulong mask = arg1;
            target_to_host_old_sigset(&set, &mask);
#else
            if (!(p = lock_user(VERIFY_READ, arg1, sizeof(target_sigset_t), 1)))
                goto efault;
            target_to_host_old_sigset(&set, p);
            unlock_user(p, arg1, 0);
#endif
            ret = get_errno(sigsuspend(&set));
        }
        break;
#endif
    case TARGET_NR_rt_sigsuspend:
        {
            sigset_t set;
            if (!(p = lock_user(VERIFY_READ, arg1, sizeof(target_sigset_t), 1)))
                goto efault;
            target_to_host_sigset(&set, p);
            unlock_user(p, arg1, 0);
            ret = get_errno(sigsuspend(&set));
        }
        break;
    case TARGET_NR_rt_sigtimedwait:
        {
            sigset_t set;
            struct timespec uts, *puts;
            siginfo_t uinfo;

            if (!(p = lock_user(VERIFY_READ, arg1, sizeof(target_sigset_t), 1)))
                goto efault;
            target_to_host_sigset(&set, p);
            unlock_user(p, arg1, 0);
            if (arg3) {
                puts = &uts;
                target_to_host_timespec(puts, arg3);
            } else {
                puts = NULL;
            }
            ret = get_errno(sigtimedwait(&set, &uinfo, puts));
            if (!is_error(ret) && arg2) {
                if (!(p = lock_user(VERIFY_WRITE, arg2, sizeof(target_siginfo_t), 0)))
                    goto efault;
                host_to_target_siginfo(p, &uinfo);
                unlock_user(p, arg2, sizeof(target_siginfo_t));
            }
        }
        break;
    case TARGET_NR_rt_sigqueueinfo:
        {
            siginfo_t uinfo;
            if (!(p = lock_user(VERIFY_READ, arg3, sizeof(target_sigset_t), 1)))
                goto efault;
            target_to_host_siginfo(&uinfo, p);
            unlock_user(p, arg1, 0);
            ret = get_errno(sys_rt_sigqueueinfo(arg1, arg2, &uinfo));
        }
        break;
#ifdef TARGET_NR_sigreturn
    case TARGET_NR_sigreturn:
        /
        ret = do_sigreturn(cpu_env);
        break;
#endif
    case TARGET_NR_rt_sigreturn:
        /
        ret = do_rt_sigreturn(cpu_env);
        break;
    case TARGET_NR_sethostname:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(sethostname(p, arg2));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_setrlimit:
        {
            int resource = arg1;
            struct target_rlimit *target_rlim;
            struct rlimit rlim;
            if (!lock_user_struct(VERIFY_READ, target_rlim, arg2, 1))
                goto efault;
            rlim.rlim_cur = target_to_host_rlim(target_rlim->rlim_cur);
            rlim.rlim_max = target_to_host_rlim(target_rlim->rlim_max);
            unlock_user_struct(target_rlim, arg2, 0);
            ret = get_errno(setrlimit(resource, &rlim));
        }
        break;
    case TARGET_NR_getrlimit:
        {
            int resource = arg1;
            struct target_rlimit *target_rlim;
            struct rlimit rlim;

            ret = get_errno(getrlimit(resource, &rlim));
            if (!is_error(ret)) {
                if (!lock_user_struct(VERIFY_WRITE, target_rlim, arg2, 0))
                    goto efault;
                target_rlim->rlim_cur = host_to_target_rlim(rlim.rlim_cur);
                target_rlim->rlim_max = host_to_target_rlim(rlim.rlim_max);
                unlock_user_struct(target_rlim, arg2, 1);
            }
        }
        break;
    case TARGET_NR_getrusage:
        {
            struct rusage rusage;
            ret = get_errno(getrusage(arg1, &rusage));
            if (!is_error(ret)) {
                host_to_target_rusage(arg2, &rusage);
            }
        }
        break;
    case TARGET_NR_gettimeofday:
        {
            struct timeval tv;
            ret = get_errno(gettimeofday(&tv, NULL));
            if (!is_error(ret)) {
                if (copy_to_user_timeval(arg1, &tv))
                    goto efault;
            }
        }
        break;
    case TARGET_NR_settimeofday:
        {
            struct timeval tv;
            if (copy_from_user_timeval(&tv, arg1))
                goto efault;
            ret = get_errno(settimeofday(&tv, NULL));
        }
        break;
#ifdef TARGET_NR_select
    case TARGET_NR_select:
        {
            struct target_sel_arg_struct *sel;
            abi_ulong inp, outp, exp, tvp;
            long nsel;

            if (!lock_user_struct(VERIFY_READ, sel, arg1, 1))
                goto efault;
            nsel = tswapl(sel->n);
            inp = tswapl(sel->inp);
            outp = tswapl(sel->outp);
            exp = tswapl(sel->exp);
            tvp = tswapl(sel->tvp);
            unlock_user_struct(sel, arg1, 0);
            ret = do_select(nsel, inp, outp, exp, tvp);
        }
        break;
#endif
#ifdef TARGET_NR_pselect6
    case TARGET_NR_pselect6:
	    goto unimplemented_nowarn;
#endif
    case TARGET_NR_symlink:
        {
            void *p2;
            p = lock_user_string(arg1);
            p2 = lock_user_string(arg2);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(symlink(p, p2));
            unlock_user(p2, arg2, 0);
            unlock_user(p, arg1, 0);
        }
        break;
#if defined(TARGET_NR_symlinkat) && defined(__NR_symlinkat)
    case TARGET_NR_symlinkat:
        {
            void *p2;
            p  = lock_user_string(arg1);
            p2 = lock_user_string(arg3);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else
                ret = get_errno(sys_symlinkat(p, arg2, p2));
            unlock_user(p2, arg3, 0);
            unlock_user(p, arg1, 0);
        }
        break;
#endif
#ifdef TARGET_NR_oldlstat
    case TARGET_NR_oldlstat:
        goto unimplemented;
#endif
    case TARGET_NR_readlink:
        {
            void *p2, *temp;
            p = lock_user_string(arg1);
            p2 = lock_user(VERIFY_WRITE, arg2, arg3, 0);
            if (!p || !p2)
                ret = -TARGET_EFAULT;
            else {
                if (strncmp((const char *)p, "/proc/self/exe", 14) == 0) {
                    char real[PATH_MAX];
                    temp = realpath(exec_path,real);
                    ret = (temp==NULL) ? get_errno(-1) : strlen(real) ;
                    snprintf((char *)p2, arg3, "%s", real);
                    }
                else
                    ret = get_errno(readlink(path(p), p2, arg3));
            }
            unlock_user(p2, arg2, ret);
            unlock_user(p, arg1, 0);
        }
        break;
#if defined(TARGET_NR_readlinkat) && defined(__NR_readlinkat)
    case TARGET_NR_readlinkat:
        {
            void *p2;
            p  = lock_user_string(arg2);
            p2 = lock_user(VERIFY_WRITE, arg3, arg4, 0);
            if (!p || !p2)
        	ret = -TARGET_EFAULT;
            else
                ret = get_errno(sys_readlinkat(arg1, path(p), p2, arg4));
            unlock_user(p2, arg3, ret);
            unlock_user(p, arg2, 0);
        }
        break;
#endif
#ifdef TARGET_NR_uselib
    case TARGET_NR_uselib:
        goto unimplemented;
#endif
#ifdef TARGET_NR_swapon
    case TARGET_NR_swapon:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(swapon(p, arg2));
        unlock_user(p, arg1, 0);
        break;
#endif
    case TARGET_NR_reboot:
        goto unimplemented;
#ifdef TARGET_NR_readdir
    case TARGET_NR_readdir:
        goto unimplemented;
#endif
#ifdef TARGET_NR_mmap
    case TARGET_NR_mmap:
#if (defined(TARGET_I386) && defined(TARGET_ABI32)) || defined(TARGET_ARM) || defined(TARGET_M68K) || defined(TARGET_CRIS) || defined(TARGET_MICROBLAZE)
        {
            abi_ulong *v;
            abi_ulong v1, v2, v3, v4, v5, v6;
            if (!(v = lock_user(VERIFY_READ, arg1, 6 * sizeof(abi_ulong), 1)))
                goto efault;
            v1 = tswapl(v[0]);
            v2 = tswapl(v[1]);
            v3 = tswapl(v[2]);
            v4 = tswapl(v[3]);
            v5 = tswapl(v[4]);
            v6 = tswapl(v[5]);
            unlock_user(v, arg1, 0);
            ret = get_errno(target_mmap(v1, v2, v3,
                                        target_to_host_bitmask(v4, mmap_flags_tbl),
                                        v5, v6));
        }
#else
        ret = get_errno(target_mmap(arg1, arg2, arg3,
                                    target_to_host_bitmask(arg4, mmap_flags_tbl),
                                    arg5,
                                    arg6));
#endif
        break;
#endif
#ifdef TARGET_NR_mmap2
    case TARGET_NR_mmap2:
#ifndef MMAP_SHIFT
#define MMAP_SHIFT 12
#endif
        ret = get_errno(target_mmap(arg1, arg2, arg3,
                                    target_to_host_bitmask(arg4, mmap_flags_tbl),
                                    arg5,
                                    arg6 << MMAP_SHIFT));
        break;
#endif
    case TARGET_NR_munmap:
        ret = get_errno(target_munmap(arg1, arg2));
        break;
    case TARGET_NR_mprotect:
        {
            TaskState *ts = ((CPUState *)cpu_env)->opaque;
            /
            if ((arg3 & PROT_GROWSDOWN)
                && arg1 >= ts->info->stack_limit
                && arg1 <= ts->info->start_stack) {
                arg3 &= ~PROT_GROWSDOWN;
                arg2 = arg2 + arg1 - ts->info->stack_limit;
                arg1 = ts->info->stack_limit;
            }
        }
        ret = get_errno(target_mprotect(arg1, arg2, arg3));
        break;
#ifdef TARGET_NR_mremap
    case TARGET_NR_mremap:
        ret = get_errno(target_mremap(arg1, arg2, arg3, arg4, arg5));
        break;
#endif
        /
#ifdef TARGET_NR_msync
    case TARGET_NR_msync:
        ret = get_errno(msync(g2h(arg1), arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_mlock
    case TARGET_NR_mlock:
        ret = get_errno(mlock(g2h(arg1), arg2));
        break;
#endif
#ifdef TARGET_NR_munlock
    case TARGET_NR_munlock:
        ret = get_errno(munlock(g2h(arg1), arg2));
        break;
#endif
#ifdef TARGET_NR_mlockall
    case TARGET_NR_mlockall:
        ret = get_errno(mlockall(arg1));
        break;
#endif
#ifdef TARGET_NR_munlockall
    case TARGET_NR_munlockall:
        ret = get_errno(munlockall());
        break;
#endif
    case TARGET_NR_truncate:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(truncate(p, arg2));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_ftruncate:
        ret = get_errno(ftruncate(arg1, arg2));
        break;
    case TARGET_NR_fchmod:
        ret = get_errno(fchmod(arg1, arg2));
        break;
#if defined(TARGET_NR_fchmodat) && defined(__NR_fchmodat)
    case TARGET_NR_fchmodat:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_fchmodat(arg1, p, arg3));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_getpriority:
        /
        ret = get_errno(sys_getpriority(arg1, arg2));
        break;
    case TARGET_NR_setpriority:
        ret = get_errno(setpriority(arg1, arg2, arg3));
        break;
#ifdef TARGET_NR_profil
    case TARGET_NR_profil:
        goto unimplemented;
#endif
    case TARGET_NR_statfs:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(statfs(path(p), &stfs));
        unlock_user(p, arg1, 0);
    convert_statfs:
        if (!is_error(ret)) {
            struct target_statfs *target_stfs;

            if (!lock_user_struct(VERIFY_WRITE, target_stfs, arg2, 0))
                goto efault;
            __put_user(stfs.f_type, &target_stfs->f_type);
            __put_user(stfs.f_bsize, &target_stfs->f_bsize);
            __put_user(stfs.f_blocks, &target_stfs->f_blocks);
            __put_user(stfs.f_bfree, &target_stfs->f_bfree);
            __put_user(stfs.f_bavail, &target_stfs->f_bavail);
            __put_user(stfs.f_files, &target_stfs->f_files);
            __put_user(stfs.f_ffree, &target_stfs->f_ffree);
            __put_user(stfs.f_fsid.__val[0], &target_stfs->f_fsid.val[0]);
            __put_user(stfs.f_fsid.__val[1], &target_stfs->f_fsid.val[1]);
            __put_user(stfs.f_namelen, &target_stfs->f_namelen);
            unlock_user_struct(target_stfs, arg2, 1);
        }
        break;
    case TARGET_NR_fstatfs:
        ret = get_errno(fstatfs(arg1, &stfs));
        goto convert_statfs;
#ifdef TARGET_NR_statfs64
    case TARGET_NR_statfs64:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(statfs(path(p), &stfs));
        unlock_user(p, arg1, 0);
    convert_statfs64:
        if (!is_error(ret)) {
            struct target_statfs64 *target_stfs;

            if (!lock_user_struct(VERIFY_WRITE, target_stfs, arg3, 0))
                goto efault;
            __put_user(stfs.f_type, &target_stfs->f_type);
            __put_user(stfs.f_bsize, &target_stfs->f_bsize);
            __put_user(stfs.f_blocks, &target_stfs->f_blocks);
            __put_user(stfs.f_bfree, &target_stfs->f_bfree);
            __put_user(stfs.f_bavail, &target_stfs->f_bavail);
            __put_user(stfs.f_files, &target_stfs->f_files);
            __put_user(stfs.f_ffree, &target_stfs->f_ffree);
            __put_user(stfs.f_fsid.__val[0], &target_stfs->f_fsid.val[0]);
            __put_user(stfs.f_fsid.__val[1], &target_stfs->f_fsid.val[1]);
            __put_user(stfs.f_namelen, &target_stfs->f_namelen);
            unlock_user_struct(target_stfs, arg3, 1);
        }
        break;
    case TARGET_NR_fstatfs64:
        ret = get_errno(fstatfs(arg1, &stfs));
        goto convert_statfs64;
#endif
#ifdef TARGET_NR_ioperm
    case TARGET_NR_ioperm:
        goto unimplemented;
#endif
#ifdef TARGET_NR_socketcall
    case TARGET_NR_socketcall:
        ret = do_socketcall(arg1, arg2);
        break;
#endif
#ifdef TARGET_NR_accept
    case TARGET_NR_accept:
        ret = do_accept(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_bind
    case TARGET_NR_bind:
        ret = do_bind(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_connect
    case TARGET_NR_connect:
        ret = do_connect(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_getpeername
    case TARGET_NR_getpeername:
        ret = do_getpeername(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_getsockname
    case TARGET_NR_getsockname:
        ret = do_getsockname(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_getsockopt
    case TARGET_NR_getsockopt:
        ret = do_getsockopt(arg1, arg2, arg3, arg4, arg5);
        break;
#endif
#ifdef TARGET_NR_listen
    case TARGET_NR_listen:
        ret = get_errno(listen(arg1, arg2));
        break;
#endif
#ifdef TARGET_NR_recv
    case TARGET_NR_recv:
        ret = do_recvfrom(arg1, arg2, arg3, arg4, 0, 0);
        break;
#endif
#ifdef TARGET_NR_recvfrom
    case TARGET_NR_recvfrom:
        ret = do_recvfrom(arg1, arg2, arg3, arg4, arg5, arg6);
        break;
#endif
#ifdef TARGET_NR_recvmsg
    case TARGET_NR_recvmsg:
        ret = do_sendrecvmsg(arg1, arg2, arg3, 0);
        break;
#endif
#ifdef TARGET_NR_send
    case TARGET_NR_send:
        ret = do_sendto(arg1, arg2, arg3, arg4, 0, 0);
        break;
#endif
#ifdef TARGET_NR_sendmsg
    case TARGET_NR_sendmsg:
        ret = do_sendrecvmsg(arg1, arg2, arg3, 1);
        break;
#endif
#ifdef TARGET_NR_sendto
    case TARGET_NR_sendto:
        ret = do_sendto(arg1, arg2, arg3, arg4, arg5, arg6);
        break;
#endif
#ifdef TARGET_NR_shutdown
    case TARGET_NR_shutdown:
        ret = get_errno(shutdown(arg1, arg2));
        break;
#endif
#ifdef TARGET_NR_socket
    case TARGET_NR_socket:
        ret = do_socket(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_socketpair
    case TARGET_NR_socketpair:
        ret = do_socketpair(arg1, arg2, arg3, arg4);
        break;
#endif
#ifdef TARGET_NR_setsockopt
    case TARGET_NR_setsockopt:
        ret = do_setsockopt(arg1, arg2, arg3, arg4, (socklen_t) arg5);
        break;
#endif

    case TARGET_NR_syslog:
        if (!(p = lock_user_string(arg2)))
            goto efault;
        ret = get_errno(sys_syslog((int)arg1, p, (int)arg3));
        unlock_user(p, arg2, 0);
        break;

    case TARGET_NR_setitimer:
        {
            struct itimerval value, ovalue, *pvalue;

            if (arg2) {
                pvalue = &value;
                if (copy_from_user_timeval(&pvalue->it_interval, arg2)
                    || copy_from_user_timeval(&pvalue->it_value,
                                              arg2 + sizeof(struct target_timeval)))
                    goto efault;
            } else {
                pvalue = NULL;
            }
            ret = get_errno(setitimer(arg1, pvalue, &ovalue));
            if (!is_error(ret) && arg3) {
                if (copy_to_user_timeval(arg3,
                                         &ovalue.it_interval)
                    || copy_to_user_timeval(arg3 + sizeof(struct target_timeval),
                                            &ovalue.it_value))
                    goto efault;
            }
        }
        break;
    case TARGET_NR_getitimer:
        {
            struct itimerval value;

            ret = get_errno(getitimer(arg1, &value));
            if (!is_error(ret) && arg2) {
                if (copy_to_user_timeval(arg2,
                                         &value.it_interval)
                    || copy_to_user_timeval(arg2 + sizeof(struct target_timeval),
                                            &value.it_value))
                    goto efault;
            }
        }
        break;
    case TARGET_NR_stat:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(stat(path(p), &st));
        unlock_user(p, arg1, 0);
        goto do_stat;
    case TARGET_NR_lstat:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(lstat(path(p), &st));
        unlock_user(p, arg1, 0);
        goto do_stat;
    case TARGET_NR_fstat:
        {
            ret = get_errno(fstat(arg1, &st));
        do_stat:
            if (!is_error(ret)) {
                struct target_stat *target_st;

                if (!lock_user_struct(VERIFY_WRITE, target_st, arg2, 0))
                    goto efault;
                memset(target_st, 0, sizeof(*target_st));
                __put_user(st.st_dev, &target_st->st_dev);
                __put_user(st.st_ino, &target_st->st_ino);
                __put_user(st.st_mode, &target_st->st_mode);
                __put_user(st.st_uid, &target_st->st_uid);
                __put_user(st.st_gid, &target_st->st_gid);
                __put_user(st.st_nlink, &target_st->st_nlink);
                __put_user(st.st_rdev, &target_st->st_rdev);
                __put_user(st.st_size, &target_st->st_size);
                __put_user(st.st_blksize, &target_st->st_blksize);
                __put_user(st.st_blocks, &target_st->st_blocks);
                __put_user(st.st_atime, &target_st->target_st_atime);
                __put_user(st.st_mtime, &target_st->target_st_mtime);
                __put_user(st.st_ctime, &target_st->target_st_ctime);
                unlock_user_struct(target_st, arg2, 1);
            }
        }
        break;
#ifdef TARGET_NR_olduname
    case TARGET_NR_olduname:
        goto unimplemented;
#endif
#ifdef TARGET_NR_iopl
    case TARGET_NR_iopl:
        goto unimplemented;
#endif
    case TARGET_NR_vhangup:
        ret = get_errno(vhangup());
        break;
#ifdef TARGET_NR_idle
    case TARGET_NR_idle:
        goto unimplemented;
#endif
#ifdef TARGET_NR_syscall
    case TARGET_NR_syscall:
    	ret = do_syscall(cpu_env,arg1 & 0xffff,arg2,arg3,arg4,arg5,arg6,0);
    	break;
#endif
    case TARGET_NR_wait4:
        {
            int status;
            abi_long status_ptr = arg2;
            struct rusage rusage, *rusage_ptr;
            abi_ulong target_rusage = arg4;
            if (target_rusage)
                rusage_ptr = &rusage;
            else
                rusage_ptr = NULL;
            ret = get_errno(wait4(arg1, &status, arg3, rusage_ptr));
            if (!is_error(ret)) {
                if (status_ptr) {
                    status = host_to_target_waitstatus(status);
                    if (put_user_s32(status, status_ptr))
                        goto efault;
                }
                if (target_rusage)
                    host_to_target_rusage(target_rusage, &rusage);
            }
        }
        break;
#ifdef TARGET_NR_swapoff
    case TARGET_NR_swapoff:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(swapoff(p));
        unlock_user(p, arg1, 0);
        break;
#endif
    case TARGET_NR_sysinfo:
        {
            struct target_sysinfo *target_value;
            struct sysinfo value;
            ret = get_errno(sysinfo(&value));
            if (!is_error(ret) && arg1)
            {
                if (!lock_user_struct(VERIFY_WRITE, target_value, arg1, 0))
                    goto efault;
                __put_user(value.uptime, &target_value->uptime);
                __put_user(value.loads[0], &target_value->loads[0]);
                __put_user(value.loads[1], &target_value->loads[1]);
                __put_user(value.loads[2], &target_value->loads[2]);
                __put_user(value.totalram, &target_value->totalram);
                __put_user(value.freeram, &target_value->freeram);
                __put_user(value.sharedram, &target_value->sharedram);
                __put_user(value.bufferram, &target_value->bufferram);
                __put_user(value.totalswap, &target_value->totalswap);
                __put_user(value.freeswap, &target_value->freeswap);
                __put_user(value.procs, &target_value->procs);
                __put_user(value.totalhigh, &target_value->totalhigh);
                __put_user(value.freehigh, &target_value->freehigh);
                __put_user(value.mem_unit, &target_value->mem_unit);
                unlock_user_struct(target_value, arg1, 1);
            }
        }
        break;
#ifdef TARGET_NR_ipc
    case TARGET_NR_ipc:
	ret = do_ipc(arg1, arg2, arg3, arg4, arg5, arg6);
	break;
#endif
#ifdef TARGET_NR_semget
    case TARGET_NR_semget:
        ret = get_errno(semget(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_semop
    case TARGET_NR_semop:
        ret = get_errno(do_semop(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_semctl
    case TARGET_NR_semctl:
        ret = do_semctl(arg1, arg2, arg3, (union target_semun)(abi_ulong)arg4);
        break;
#endif
#ifdef TARGET_NR_msgctl
    case TARGET_NR_msgctl:
        ret = do_msgctl(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_msgget
    case TARGET_NR_msgget:
        ret = get_errno(msgget(arg1, arg2));
        break;
#endif
#ifdef TARGET_NR_msgrcv
    case TARGET_NR_msgrcv:
        ret = do_msgrcv(arg1, arg2, arg3, arg4, arg5);
        break;
#endif
#ifdef TARGET_NR_msgsnd
    case TARGET_NR_msgsnd:
        ret = do_msgsnd(arg1, arg2, arg3, arg4);
        break;
#endif
#ifdef TARGET_NR_shmget
    case TARGET_NR_shmget:
        ret = get_errno(shmget(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_shmctl
    case TARGET_NR_shmctl:
        ret = do_shmctl(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_shmat
    case TARGET_NR_shmat:
        ret = do_shmat(arg1, arg2, arg3);
        break;
#endif
#ifdef TARGET_NR_shmdt
    case TARGET_NR_shmdt:
        ret = do_shmdt(arg1);
        break;
#endif
    case TARGET_NR_fsync:
        ret = get_errno(fsync(arg1));
        break;
    case TARGET_NR_clone:
#if defined(TARGET_SH4) || defined(TARGET_ALPHA)
        ret = get_errno(do_fork(cpu_env, arg1, arg2, arg3, arg5, arg4));
#elif defined(TARGET_CRIS)
        ret = get_errno(do_fork(cpu_env, arg2, arg1, arg3, arg4, arg5));
#else
        ret = get_errno(do_fork(cpu_env, arg1, arg2, arg3, arg4, arg5));
#endif
        break;
#ifdef __NR_exit_group
        /
    case TARGET_NR_exit_group:
#ifdef TARGET_GPROF
        _mcleanup();
#endif
        gdb_exit(cpu_env, arg1);
        ret = get_errno(exit_group(arg1));
        break;
#endif
    case TARGET_NR_setdomainname:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(setdomainname(p, arg2));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_uname:
        /
        {
            struct new_utsname * buf;

            if (!lock_user_struct(VERIFY_WRITE, buf, arg1, 0))
                goto efault;
            ret = get_errno(sys_uname(buf));
            if (!is_error(ret)) {
                /
                strcpy (buf->machine, cpu_to_uname_machine(cpu_env));
                /
                if (qemu_uname_release && *qemu_uname_release)
                  strcpy (buf->release, qemu_uname_release);
            }
            unlock_user_struct(buf, arg1, 1);
        }
        break;
#ifdef TARGET_I386
    case TARGET_NR_modify_ldt:
        ret = do_modify_ldt(cpu_env, arg1, arg2, arg3);
        break;
#if !defined(TARGET_X86_64)
    case TARGET_NR_vm86old:
        goto unimplemented;
    case TARGET_NR_vm86:
        ret = do_vm86(cpu_env, arg1, arg2);
        break;
#endif
#endif
    case TARGET_NR_adjtimex:
        goto unimplemented;
#ifdef TARGET_NR_create_module
    case TARGET_NR_create_module:
#endif
    case TARGET_NR_init_module:
    case TARGET_NR_delete_module:
#ifdef TARGET_NR_get_kernel_syms
    case TARGET_NR_get_kernel_syms:
#endif
        goto unimplemented;
    case TARGET_NR_quotactl:
        goto unimplemented;
    case TARGET_NR_getpgid:
        ret = get_errno(getpgid(arg1));
        break;
    case TARGET_NR_fchdir:
        ret = get_errno(fchdir(arg1));
        break;
#ifdef TARGET_NR_bdflush /
    case TARGET_NR_bdflush:
        goto unimplemented;
#endif
#ifdef TARGET_NR_sysfs
    case TARGET_NR_sysfs:
        goto unimplemented;
#endif
    case TARGET_NR_personality:
        ret = get_errno(personality(arg1));
        break;
#ifdef TARGET_NR_afs_syscall
    case TARGET_NR_afs_syscall:
        goto unimplemented;
#endif
#ifdef TARGET_NR__llseek /
    case TARGET_NR__llseek:
        {
            int64_t res;
#if !defined(__NR_llseek)
            res = lseek(arg1, ((uint64_t)arg2 << 32) | arg3, arg5);
            if (res == -1) {
                ret = get_errno(res);
            } else {
                ret = 0;
            }
#else
            ret = get_errno(_llseek(arg1, arg2, arg3, &res, arg5));
#endif
            if ((ret == 0) && put_user_s64(res, arg4)) {
                goto efault;
            }
        }
        break;
#endif
    case TARGET_NR_getdents:
#if TARGET_ABI_BITS == 32 && HOST_LONG_BITS == 64
        {
            struct target_dirent *target_dirp;
            struct linux_dirent *dirp;
            abi_long count = arg3;

	    dirp = malloc(count);
	    if (!dirp) {
                ret = -TARGET_ENOMEM;
                goto fail;
            }

            ret = get_errno(sys_getdents(arg1, dirp, count));
            if (!is_error(ret)) {
                struct linux_dirent *de;
		struct target_dirent *tde;
                int len = ret;
                int reclen, treclen;
		int count1, tnamelen;

		count1 = 0;
                de = dirp;
                if (!(target_dirp = lock_user(VERIFY_WRITE, arg2, count, 0)))
                    goto efault;
		tde = target_dirp;
                while (len > 0) {
                    reclen = de->d_reclen;
		    treclen = reclen - (2 * (sizeof(long) - sizeof(abi_long)));
                    tde->d_reclen = tswap16(treclen);
                    tde->d_ino = tswapl(de->d_ino);
                    tde->d_off = tswapl(de->d_off);
		    tnamelen = treclen - (2 * sizeof(abi_long) + 2);
		    if (tnamelen > 256)
                        tnamelen = 256;
                    /
                    pstrcpy(tde->d_name, tnamelen, de->d_name);
                    de = (struct linux_dirent *)((char *)de + reclen);
                    len -= reclen;
                    tde = (struct target_dirent *)((char *)tde + treclen);
		    count1 += treclen;
                }
		ret = count1;
                unlock_user(target_dirp, arg2, ret);
            }
	    free(dirp);
        }
#else
        {
            struct linux_dirent *dirp;
            abi_long count = arg3;

            if (!(dirp = lock_user(VERIFY_WRITE, arg2, count, 0)))
                goto efault;
            ret = get_errno(sys_getdents(arg1, dirp, count));
            if (!is_error(ret)) {
                struct linux_dirent *de;
                int len = ret;
                int reclen;
                de = dirp;
                while (len > 0) {
                    reclen = de->d_reclen;
                    if (reclen > len)
                        break;
                    de->d_reclen = tswap16(reclen);
                    tswapls(&de->d_ino);
                    tswapls(&de->d_off);
                    de = (struct linux_dirent *)((char *)de + reclen);
                    len -= reclen;
                }
            }
            unlock_user(dirp, arg2, ret);
        }
#endif
        break;
#if defined(TARGET_NR_getdents64) && defined(__NR_getdents64)
    case TARGET_NR_getdents64:
        {
            struct linux_dirent64 *dirp;
            abi_long count = arg3;
            if (!(dirp = lock_user(VERIFY_WRITE, arg2, count, 0)))
                goto efault;
            ret = get_errno(sys_getdents64(arg1, dirp, count));
            if (!is_error(ret)) {
                struct linux_dirent64 *de;
                int len = ret;
                int reclen;
                de = dirp;
                while (len > 0) {
                    reclen = de->d_reclen;
                    if (reclen > len)
                        break;
                    de->d_reclen = tswap16(reclen);
                    tswap64s((uint64_t *)&de->d_ino);
                    tswap64s((uint64_t *)&de->d_off);
                    de = (struct linux_dirent64 *)((char *)de + reclen);
                    len -= reclen;
                }
            }
            unlock_user(dirp, arg2, ret);
        }
        break;
#endif /
#ifdef TARGET_NR__newselect
    case TARGET_NR__newselect:
        ret = do_select(arg1, arg2, arg3, arg4, arg5);
        break;
#endif
#if defined(TARGET_NR_poll) || defined(TARGET_NR_ppoll)
# ifdef TARGET_NR_poll
    case TARGET_NR_poll:
# endif
# ifdef TARGET_NR_ppoll
    case TARGET_NR_ppoll:
# endif
        {
            struct target_pollfd *target_pfd;
            unsigned int nfds = arg2;
            int timeout = arg3;
            struct pollfd *pfd;
            unsigned int i;

            target_pfd = lock_user(VERIFY_WRITE, arg1, sizeof(struct target_pollfd) * nfds, 1);
            if (!target_pfd)
                goto efault;

            pfd = alloca(sizeof(struct pollfd) * nfds);
            for(i = 0; i < nfds; i++) {
                pfd[i].fd = tswap32(target_pfd[i].fd);
                pfd[i].events = tswap16(target_pfd[i].events);
            }

# ifdef TARGET_NR_ppoll
            if (num == TARGET_NR_ppoll) {
                struct timespec _timeout_ts, *timeout_ts = &_timeout_ts;
                target_sigset_t *target_set;
                sigset_t _set, *set = &_set;

                if (arg3) {
                    if (target_to_host_timespec(timeout_ts, arg3)) {
                        unlock_user(target_pfd, arg1, 0);
                        goto efault;
                    }
                } else {
                    timeout_ts = NULL;
                }

                if (arg4) {
                    target_set = lock_user(VERIFY_READ, arg4, sizeof(target_sigset_t), 1);
                    if (!target_set) {
                        unlock_user(target_pfd, arg1, 0);
                        goto efault;
                    }
                    target_to_host_sigset(set, target_set);
                } else {
                    set = NULL;
                }

                ret = get_errno(sys_ppoll(pfd, nfds, timeout_ts, set, _NSIG/8));

                if (!is_error(ret) && arg3) {
                    host_to_target_timespec(arg3, timeout_ts);
                }
                if (arg4) {
                    unlock_user(target_set, arg4, 0);
                }
            } else
# endif
                ret = get_errno(poll(pfd, nfds, timeout));

            if (!is_error(ret)) {
                for(i = 0; i < nfds; i++) {
                    target_pfd[i].revents = tswap16(pfd[i].revents);
                }
            }
            unlock_user(target_pfd, arg1, sizeof(struct target_pollfd) * nfds);
        }
        break;
#endif
    case TARGET_NR_flock:
        /
        ret = get_errno(flock(arg1, arg2));
        break;
    case TARGET_NR_readv:
        {
            int count = arg3;
            struct iovec *vec;

            vec = alloca(count * sizeof(struct iovec));
            if (lock_iovec(VERIFY_WRITE, vec, arg2, count, 0) < 0)
                goto efault;
            ret = get_errno(readv(arg1, vec, count));
            unlock_iovec(vec, arg2, count, 1);
        }
        break;
    case TARGET_NR_writev:
        {
            int count = arg3;
            struct iovec *vec;

            vec = alloca(count * sizeof(struct iovec));
            if (lock_iovec(VERIFY_READ, vec, arg2, count, 1) < 0)
                goto efault;
            ret = get_errno(writev(arg1, vec, count));
            unlock_iovec(vec, arg2, count, 0);
        }
        break;
    case TARGET_NR_getsid:
        ret = get_errno(getsid(arg1));
        break;
#if defined(TARGET_NR_fdatasync) /
    case TARGET_NR_fdatasync:
        ret = get_errno(fdatasync(arg1));
        break;
#endif
    case TARGET_NR__sysctl:
        /
        ret = -TARGET_ENOTDIR;
        break;
    case TARGET_NR_sched_getaffinity:
        {
            unsigned int mask_size;
            unsigned long *mask;

            /
            if (arg2 & (sizeof(abi_ulong) - 1)) {
                ret = -TARGET_EINVAL;
                break;
            }
            mask_size = (arg2 + (sizeof(*mask) - 1)) & ~(sizeof(*mask) - 1);

            mask = alloca(mask_size);
            ret = get_errno(sys_sched_getaffinity(arg1, mask_size, mask));

            if (!is_error(ret)) {
                if (arg2 > ret) {
                    /
                    unsigned long zero = arg2 - ret;
                    p = alloca(zero);
                    memset(p, 0, zero);
                    if (copy_to_user(arg3 + zero, p, zero)) {
                        goto efault;
                    }
                    arg2 = ret;
                }
                if (copy_to_user(arg3, mask, arg2)) {
                    goto efault;
                }
                ret = arg2;
            }
        }
        break;
    case TARGET_NR_sched_setaffinity:
        {
            unsigned int mask_size;
            unsigned long *mask;

            /
            if (arg2 & (sizeof(abi_ulong) - 1)) {
                ret = -TARGET_EINVAL;
                break;
            }
            mask_size = (arg2 + (sizeof(*mask) - 1)) & ~(sizeof(*mask) - 1);

            mask = alloca(mask_size);
            if (!lock_user_struct(VERIFY_READ, p, arg3, 1)) {
                goto efault;
            }
            memcpy(mask, p, arg2);
            unlock_user_struct(p, arg2, 0);

            ret = get_errno(sys_sched_setaffinity(arg1, mask_size, mask));
        }
        break;
    case TARGET_NR_sched_setparam:
        {
            struct sched_param *target_schp;
            struct sched_param schp;

            if (!lock_user_struct(VERIFY_READ, target_schp, arg2, 1))
                goto efault;
            schp.sched_priority = tswap32(target_schp->sched_priority);
            unlock_user_struct(target_schp, arg2, 0);
            ret = get_errno(sched_setparam(arg1, &schp));
        }
        break;
    case TARGET_NR_sched_getparam:
        {
            struct sched_param *target_schp;
            struct sched_param schp;
            ret = get_errno(sched_getparam(arg1, &schp));
            if (!is_error(ret)) {
                if (!lock_user_struct(VERIFY_WRITE, target_schp, arg2, 0))
                    goto efault;
                target_schp->sched_priority = tswap32(schp.sched_priority);
                unlock_user_struct(target_schp, arg2, 1);
            }
        }
        break;
    case TARGET_NR_sched_setscheduler:
        {
            struct sched_param *target_schp;
            struct sched_param schp;
            if (!lock_user_struct(VERIFY_READ, target_schp, arg3, 1))
                goto efault;
            schp.sched_priority = tswap32(target_schp->sched_priority);
            unlock_user_struct(target_schp, arg3, 0);
            ret = get_errno(sched_setscheduler(arg1, arg2, &schp));
        }
        break;
    case TARGET_NR_sched_getscheduler:
        ret = get_errno(sched_getscheduler(arg1));
        break;
    case TARGET_NR_sched_yield:
        ret = get_errno(sched_yield());
        break;
    case TARGET_NR_sched_get_priority_max:
        ret = get_errno(sched_get_priority_max(arg1));
        break;
    case TARGET_NR_sched_get_priority_min:
        ret = get_errno(sched_get_priority_min(arg1));
        break;
    case TARGET_NR_sched_rr_get_interval:
        {
            struct timespec ts;
            ret = get_errno(sched_rr_get_interval(arg1, &ts));
            if (!is_error(ret)) {
                host_to_target_timespec(arg2, &ts);
            }
        }
        break;
    case TARGET_NR_nanosleep:
        {
            struct timespec req, rem;
            target_to_host_timespec(&req, arg1);
            ret = get_errno(nanosleep(&req, &rem));
            if (is_error(ret) && arg2) {
                host_to_target_timespec(arg2, &rem);
            }
        }
        break;
#ifdef TARGET_NR_query_module
    case TARGET_NR_query_module:
        goto unimplemented;
#endif
#ifdef TARGET_NR_nfsservctl
    case TARGET_NR_nfsservctl:
        goto unimplemented;
#endif
    case TARGET_NR_prctl:
        switch (arg1)
            {
            case PR_GET_PDEATHSIG:
                {
                    int deathsig;
                    ret = get_errno(prctl(arg1, &deathsig, arg3, arg4, arg5));
                    if (!is_error(ret) && arg2
                        && put_user_ual(deathsig, arg2))
                        goto efault;
                }
                break;
            default:
                ret = get_errno(prctl(arg1, arg2, arg3, arg4, arg5));
                break;
            }
        break;
#ifdef TARGET_NR_arch_prctl
    case TARGET_NR_arch_prctl:
#if defined(TARGET_I386) && !defined(TARGET_ABI32)
        ret = do_arch_prctl(cpu_env, arg1, arg2);
        break;
#else
        goto unimplemented;
#endif
#endif
#ifdef TARGET_NR_pread
    case TARGET_NR_pread:
#ifdef TARGET_ARM
        if (((CPUARMState *)cpu_env)->eabi)
            arg4 = arg5;
#endif
        if (!(p = lock_user(VERIFY_WRITE, arg2, arg3, 0)))
            goto efault;
        ret = get_errno(pread(arg1, p, arg3, arg4));
        unlock_user(p, arg2, ret);
        break;
    case TARGET_NR_pwrite:
#ifdef TARGET_ARM
        if (((CPUARMState *)cpu_env)->eabi)
            arg4 = arg5;
#endif
        if (!(p = lock_user(VERIFY_READ, arg2, arg3, 1)))
            goto efault;
        ret = get_errno(pwrite(arg1, p, arg3, arg4));
        unlock_user(p, arg2, 0);
        break;
#endif
#ifdef TARGET_NR_pread64
    case TARGET_NR_pread64:
        if (!(p = lock_user(VERIFY_WRITE, arg2, arg3, 0)))
            goto efault;
        ret = get_errno(pread64(arg1, p, arg3, target_offset64(arg4, arg5)));
        unlock_user(p, arg2, ret);
        break;
    case TARGET_NR_pwrite64:
        if (!(p = lock_user(VERIFY_READ, arg2, arg3, 1)))
            goto efault;
        ret = get_errno(pwrite64(arg1, p, arg3, target_offset64(arg4, arg5)));
        unlock_user(p, arg2, 0);
        break;
#endif
    case TARGET_NR_getcwd:
        if (!(p = lock_user(VERIFY_WRITE, arg1, arg2, 0)))
            goto efault;
        ret = get_errno(sys_getcwd1(p, arg2));
        unlock_user(p, arg1, ret);
        break;
    case TARGET_NR_capget:
        goto unimplemented;
    case TARGET_NR_capset:
        goto unimplemented;
    case TARGET_NR_sigaltstack:
#if defined(TARGET_I386) || defined(TARGET_ARM) || defined(TARGET_MIPS) || \
    defined(TARGET_SPARC) || defined(TARGET_PPC) || defined(TARGET_ALPHA) || \
    defined(TARGET_M68K)
        ret = do_sigaltstack(arg1, arg2, get_sp_from_cpustate((CPUState *)cpu_env));
        break;
#else
        goto unimplemented;
#endif
    case TARGET_NR_sendfile:
        goto unimplemented;
#ifdef TARGET_NR_getpmsg
    case TARGET_NR_getpmsg:
        goto unimplemented;
#endif
#ifdef TARGET_NR_putpmsg
    case TARGET_NR_putpmsg:
        goto unimplemented;
#endif
#ifdef TARGET_NR_vfork
    case TARGET_NR_vfork:
        ret = get_errno(do_fork(cpu_env, CLONE_VFORK | CLONE_VM | SIGCHLD,
                        0, 0, 0, 0));
        break;
#endif
#ifdef TARGET_NR_ugetrlimit
    case TARGET_NR_ugetrlimit:
    {
	struct rlimit rlim;
	ret = get_errno(getrlimit(arg1, &rlim));
	if (!is_error(ret)) {
	    struct target_rlimit *target_rlim;
            if (!lock_user_struct(VERIFY_WRITE, target_rlim, arg2, 0))
                goto efault;
	    target_rlim->rlim_cur = host_to_target_rlim(rlim.rlim_cur);
	    target_rlim->rlim_max = host_to_target_rlim(rlim.rlim_max);
            unlock_user_struct(target_rlim, arg2, 1);
	}
	break;
    }
#endif
#ifdef TARGET_NR_truncate64
    case TARGET_NR_truncate64:
        if (!(p = lock_user_string(arg1)))
            goto efault;
	ret = target_truncate64(cpu_env, p, arg2, arg3, arg4);
        unlock_user(p, arg1, 0);
	break;
#endif
#ifdef TARGET_NR_ftruncate64
    case TARGET_NR_ftruncate64:
	ret = target_ftruncate64(cpu_env, arg1, arg2, arg3, arg4);
	break;
#endif
#ifdef TARGET_NR_stat64
    case TARGET_NR_stat64:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(stat(path(p), &st));
        unlock_user(p, arg1, 0);
        if (!is_error(ret))
            ret = host_to_target_stat64(cpu_env, arg2, &st);
        break;
#endif
#ifdef TARGET_NR_lstat64
    case TARGET_NR_lstat64:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(lstat(path(p), &st));
        unlock_user(p, arg1, 0);
        if (!is_error(ret))
            ret = host_to_target_stat64(cpu_env, arg2, &st);
        break;
#endif
#ifdef TARGET_NR_fstat64
    case TARGET_NR_fstat64:
        ret = get_errno(fstat(arg1, &st));
        if (!is_error(ret))
            ret = host_to_target_stat64(cpu_env, arg2, &st);
        break;
#endif
#if (defined(TARGET_NR_fstatat64) || defined(TARGET_NR_newfstatat)) && \
        (defined(__NR_fstatat64) || defined(__NR_newfstatat))
#ifdef TARGET_NR_fstatat64
    case TARGET_NR_fstatat64:
#endif
#ifdef TARGET_NR_newfstatat
    case TARGET_NR_newfstatat:
#endif
        if (!(p = lock_user_string(arg2)))
            goto efault;
#ifdef __NR_fstatat64
        ret = get_errno(sys_fstatat64(arg1, path(p), &st, arg4));
#else
        ret = get_errno(sys_newfstatat(arg1, path(p), &st, arg4));
#endif
        if (!is_error(ret))
            ret = host_to_target_stat64(cpu_env, arg3, &st);
        break;
#endif
    case TARGET_NR_lchown:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(lchown(p, low2highuid(arg2), low2highgid(arg3)));
        unlock_user(p, arg1, 0);
        break;
#ifdef TARGET_NR_getuid
    case TARGET_NR_getuid:
        ret = get_errno(high2lowuid(getuid()));
        break;
#endif
#ifdef TARGET_NR_getgid
    case TARGET_NR_getgid:
        ret = get_errno(high2lowgid(getgid()));
        break;
#endif
#ifdef TARGET_NR_geteuid
    case TARGET_NR_geteuid:
        ret = get_errno(high2lowuid(geteuid()));
        break;
#endif
#ifdef TARGET_NR_getegid
    case TARGET_NR_getegid:
        ret = get_errno(high2lowgid(getegid()));
        break;
#endif
    case TARGET_NR_setreuid:
        ret = get_errno(setreuid(low2highuid(arg1), low2highuid(arg2)));
        break;
    case TARGET_NR_setregid:
        ret = get_errno(setregid(low2highgid(arg1), low2highgid(arg2)));
        break;
    case TARGET_NR_getgroups:
        {
            int gidsetsize = arg1;
            target_id *target_grouplist;
            gid_t *grouplist;
            int i;

            grouplist = alloca(gidsetsize * sizeof(gid_t));
            ret = get_errno(getgroups(gidsetsize, grouplist));
            if (gidsetsize == 0)
                break;
            if (!is_error(ret)) {
                target_grouplist = lock_user(VERIFY_WRITE, arg2, gidsetsize * 2, 0);
                if (!target_grouplist)
                    goto efault;
                for(i = 0;i < ret; i++)
                    target_grouplist[i] = tswapid(high2lowgid(grouplist[i]));
                unlock_user(target_grouplist, arg2, gidsetsize * 2);
            }
        }
        break;
    case TARGET_NR_setgroups:
        {
            int gidsetsize = arg1;
            target_id *target_grouplist;
            gid_t *grouplist;
            int i;

            grouplist = alloca(gidsetsize * sizeof(gid_t));
            target_grouplist = lock_user(VERIFY_READ, arg2, gidsetsize * 2, 1);
            if (!target_grouplist) {
                ret = -TARGET_EFAULT;
                goto fail;
            }
            for(i = 0;i < gidsetsize; i++)
                grouplist[i] = low2highgid(tswapid(target_grouplist[i]));
            unlock_user(target_grouplist, arg2, 0);
            ret = get_errno(setgroups(gidsetsize, grouplist));
        }
        break;
    case TARGET_NR_fchown:
        ret = get_errno(fchown(arg1, low2highuid(arg2), low2highgid(arg3)));
        break;
#if defined(TARGET_NR_fchownat) && defined(__NR_fchownat)
    case TARGET_NR_fchownat:
        if (!(p = lock_user_string(arg2))) 
            goto efault;
        ret = get_errno(sys_fchownat(arg1, p, low2highuid(arg3), low2highgid(arg4), arg5));
        unlock_user(p, arg2, 0);
        break;
#endif
#ifdef TARGET_NR_setresuid
    case TARGET_NR_setresuid:
        ret = get_errno(setresuid(low2highuid(arg1),
                                  low2highuid(arg2),
                                  low2highuid(arg3)));
        break;
#endif
#ifdef TARGET_NR_getresuid
    case TARGET_NR_getresuid:
        {
            uid_t ruid, euid, suid;
            ret = get_errno(getresuid(&ruid, &euid, &suid));
            if (!is_error(ret)) {
                if (put_user_u16(high2lowuid(ruid), arg1)
                    || put_user_u16(high2lowuid(euid), arg2)
                    || put_user_u16(high2lowuid(suid), arg3))
                    goto efault;
            }
        }
        break;
#endif
#ifdef TARGET_NR_getresgid
    case TARGET_NR_setresgid:
        ret = get_errno(setresgid(low2highgid(arg1),
                                  low2highgid(arg2),
                                  low2highgid(arg3)));
        break;
#endif
#ifdef TARGET_NR_getresgid
    case TARGET_NR_getresgid:
        {
            gid_t rgid, egid, sgid;
            ret = get_errno(getresgid(&rgid, &egid, &sgid));
            if (!is_error(ret)) {
                if (put_user_u16(high2lowgid(rgid), arg1)
                    || put_user_u16(high2lowgid(egid), arg2)
                    || put_user_u16(high2lowgid(sgid), arg3))
                    goto efault;
            }
        }
        break;
#endif
    case TARGET_NR_chown:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(chown(p, low2highuid(arg2), low2highgid(arg3)));
        unlock_user(p, arg1, 0);
        break;
    case TARGET_NR_setuid:
        ret = get_errno(setuid(low2highuid(arg1)));
        break;
    case TARGET_NR_setgid:
        ret = get_errno(setgid(low2highgid(arg1)));
        break;
    case TARGET_NR_setfsuid:
        ret = get_errno(setfsuid(arg1));
        break;
    case TARGET_NR_setfsgid:
        ret = get_errno(setfsgid(arg1));
        break;

#ifdef TARGET_NR_lchown32
    case TARGET_NR_lchown32:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(lchown(p, arg2, arg3));
        unlock_user(p, arg1, 0);
        break;
#endif
#ifdef TARGET_NR_getuid32
    case TARGET_NR_getuid32:
        ret = get_errno(getuid());
        break;
#endif

#if defined(TARGET_NR_getxuid) && defined(TARGET_ALPHA)
   /
    case TARGET_NR_getxuid:
         {
            uid_t euid;
            euid=geteuid();
            ((CPUAlphaState *)cpu_env)->ir[IR_A4]=euid;
         }
        ret = get_errno(getuid());
        break;
#endif
#if defined(TARGET_NR_getxgid) && defined(TARGET_ALPHA)
   /
    case TARGET_NR_getxgid:
         {
            uid_t egid;
            egid=getegid();
            ((CPUAlphaState *)cpu_env)->ir[IR_A4]=egid;
         }
        ret = get_errno(getgid());
        break;
#endif
#if defined(TARGET_NR_osf_getsysinfo) && defined(TARGET_ALPHA)
    /
    case TARGET_NR_osf_getsysinfo:
        ret = -TARGET_EOPNOTSUPP;
        switch (arg1) {
          case TARGET_GSI_IEEE_FP_CONTROL:
            {
                uint64_t swcr, fpcr = cpu_alpha_load_fpcr (cpu_env);

                /
                swcr = (fpcr >> 35) & SWCR_STATUS_MASK;
                swcr |= (fpcr >> 36) & SWCR_MAP_DMZ;
                swcr |= (~fpcr >> 48) & (SWCR_TRAP_ENABLE_INV
                                        | SWCR_TRAP_ENABLE_DZE
                                        | SWCR_TRAP_ENABLE_OVF);
                swcr |= (~fpcr >> 57) & (SWCR_TRAP_ENABLE_UNF
                                        | SWCR_TRAP_ENABLE_INE);
                swcr |= (fpcr >> 47) & SWCR_MAP_UMZ;
                swcr |= (~fpcr >> 41) & SWCR_TRAP_ENABLE_DNO;

                if (put_user_u64 (swcr, arg2))
                        goto efault;
                ret = 0;
            }
            break;

          /
        }
        break;
#endif
#if defined(TARGET_NR_osf_setsysinfo) && defined(TARGET_ALPHA)
    /
    case TARGET_NR_osf_setsysinfo:
        ret = -TARGET_EOPNOTSUPP;
        switch (arg1) {
          case TARGET_SSI_IEEE_FP_CONTROL:
          case TARGET_SSI_IEEE_RAISE_EXCEPTION:
            {
                uint64_t swcr, fpcr, orig_fpcr;

                if (get_user_u64 (swcr, arg2))
                    goto efault;
                orig_fpcr = cpu_alpha_load_fpcr (cpu_env);
                fpcr = orig_fpcr & FPCR_DYN_MASK;

                /
                fpcr |= (swcr & SWCR_STATUS_MASK) << 35;
                fpcr |= (swcr & SWCR_MAP_DMZ) << 36;
                fpcr |= (~swcr & (SWCR_TRAP_ENABLE_INV
                                  | SWCR_TRAP_ENABLE_DZE
                                  | SWCR_TRAP_ENABLE_OVF)) << 48;
                fpcr |= (~swcr & (SWCR_TRAP_ENABLE_UNF
                                  | SWCR_TRAP_ENABLE_INE)) << 57;
                fpcr |= (swcr & SWCR_MAP_UMZ ? FPCR_UNDZ | FPCR_UNFD : 0);
                fpcr |= (~swcr & SWCR_TRAP_ENABLE_DNO) << 41;

                cpu_alpha_store_fpcr (cpu_env, fpcr);
                ret = 0;

                if (arg1 == TARGET_SSI_IEEE_RAISE_EXCEPTION) {
                    /
                    fpcr &= ~(orig_fpcr & FPCR_STATUS_MASK);

                    /
                    /
                }
            }
            break;

          /
        }
        break;
#endif
#ifdef TARGET_NR_osf_sigprocmask
    /
    case TARGET_NR_osf_sigprocmask:
        {
            abi_ulong mask;
            int how = arg1;
            sigset_t set, oldset;

            switch(arg1) {
            case TARGET_SIG_BLOCK:
                how = SIG_BLOCK;
                break;
            case TARGET_SIG_UNBLOCK:
                how = SIG_UNBLOCK;
                break;
            case TARGET_SIG_SETMASK:
                how = SIG_SETMASK;
                break;
            default:
                ret = -TARGET_EINVAL;
                goto fail;
            }
            mask = arg2;
            target_to_host_old_sigset(&set, &mask);
            sigprocmask(arg1, &set, &oldset);
            host_to_target_old_sigset(&mask, &oldset);
            ret = mask;
        }
        break;
#endif

#ifdef TARGET_NR_getgid32
    case TARGET_NR_getgid32:
        ret = get_errno(getgid());
        break;
#endif
#ifdef TARGET_NR_geteuid32
    case TARGET_NR_geteuid32:
        ret = get_errno(geteuid());
        break;
#endif
#ifdef TARGET_NR_getegid32
    case TARGET_NR_getegid32:
        ret = get_errno(getegid());
        break;
#endif
#ifdef TARGET_NR_setreuid32
    case TARGET_NR_setreuid32:
        ret = get_errno(setreuid(arg1, arg2));
        break;
#endif
#ifdef TARGET_NR_setregid32
    case TARGET_NR_setregid32:
        ret = get_errno(setregid(arg1, arg2));
        break;
#endif
#ifdef TARGET_NR_getgroups32
    case TARGET_NR_getgroups32:
        {
            int gidsetsize = arg1;
            uint32_t *target_grouplist;
            gid_t *grouplist;
            int i;

            grouplist = alloca(gidsetsize * sizeof(gid_t));
            ret = get_errno(getgroups(gidsetsize, grouplist));
            if (gidsetsize == 0)
                break;
            if (!is_error(ret)) {
                target_grouplist = lock_user(VERIFY_WRITE, arg2, gidsetsize * 4, 0);
                if (!target_grouplist) {
                    ret = -TARGET_EFAULT;
                    goto fail;
                }
                for(i = 0;i < ret; i++)
                    target_grouplist[i] = tswap32(grouplist[i]);
                unlock_user(target_grouplist, arg2, gidsetsize * 4);
            }
        }
        break;
#endif
#ifdef TARGET_NR_setgroups32
    case TARGET_NR_setgroups32:
        {
            int gidsetsize = arg1;
            uint32_t *target_grouplist;
            gid_t *grouplist;
            int i;

            grouplist = alloca(gidsetsize * sizeof(gid_t));
            target_grouplist = lock_user(VERIFY_READ, arg2, gidsetsize * 4, 1);
            if (!target_grouplist) {
                ret = -TARGET_EFAULT;
                goto fail;
            }
            for(i = 0;i < gidsetsize; i++)
                grouplist[i] = tswap32(target_grouplist[i]);
            unlock_user(target_grouplist, arg2, 0);
            ret = get_errno(setgroups(gidsetsize, grouplist));
        }
        break;
#endif
#ifdef TARGET_NR_fchown32
    case TARGET_NR_fchown32:
        ret = get_errno(fchown(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_setresuid32
    case TARGET_NR_setresuid32:
        ret = get_errno(setresuid(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_getresuid32
    case TARGET_NR_getresuid32:
        {
            uid_t ruid, euid, suid;
            ret = get_errno(getresuid(&ruid, &euid, &suid));
            if (!is_error(ret)) {
                if (put_user_u32(ruid, arg1)
                    || put_user_u32(euid, arg2)
                    || put_user_u32(suid, arg3))
                    goto efault;
            }
        }
        break;
#endif
#ifdef TARGET_NR_setresgid32
    case TARGET_NR_setresgid32:
        ret = get_errno(setresgid(arg1, arg2, arg3));
        break;
#endif
#ifdef TARGET_NR_getresgid32
    case TARGET_NR_getresgid32:
        {
            gid_t rgid, egid, sgid;
            ret = get_errno(getresgid(&rgid, &egid, &sgid));
            if (!is_error(ret)) {
                if (put_user_u32(rgid, arg1)
                    || put_user_u32(egid, arg2)
                    || put_user_u32(sgid, arg3))
                    goto efault;
            }
        }
        break;
#endif
#ifdef TARGET_NR_chown32
    case TARGET_NR_chown32:
        if (!(p = lock_user_string(arg1)))
            goto efault;
        ret = get_errno(chown(p, arg2, arg3));
        unlock_user(p, arg1, 0);
        break;
#endif
#ifdef TARGET_NR_setuid32
    case TARGET_NR_setuid32:
        ret = get_errno(setuid(arg1));
        break;
#endif
#ifdef TARGET_NR_setgid32
    case TARGET_NR_setgid32:
        ret = get_errno(setgid(arg1));
        break;
#endif
#ifdef TARGET_NR_setfsuid32
    case TARGET_NR_setfsuid32:
        ret = get_errno(setfsuid(arg1));
        break;
#endif
#ifdef TARGET_NR_setfsgid32
    case TARGET_NR_setfsgid32:
        ret = get_errno(setfsgid(arg1));
        break;
#endif

    case TARGET_NR_pivot_root:
        goto unimplemented;
#ifdef TARGET_NR_mincore
    case TARGET_NR_mincore:
        {
            void *a;
            ret = -TARGET_EFAULT;
            if (!(a = lock_user(VERIFY_READ, arg1,arg2, 0)))
                goto efault;
            if (!(p = lock_user_string(arg3)))
                goto mincore_fail;
            ret = get_errno(mincore(a, arg2, p));
            unlock_user(p, arg3, ret);
            mincore_fail:
            unlock_user(a, arg1, 0);
        }
        break;
#endif
#ifdef TARGET_NR_arm_fadvise64_64
    case TARGET_NR_arm_fadvise64_64:
	{
		/
		abi_long temp;
		temp = arg3;
		arg3 = arg4;
		arg4 = temp;
	}
#endif
#if defined(TARGET_NR_fadvise64_64) || defined(TARGET_NR_arm_fadvise64_64) || defined(TARGET_NR_fadvise64)
#ifdef TARGET_NR_fadvise64_64
    case TARGET_NR_fadvise64_64:
#endif
#ifdef TARGET_NR_fadvise64
    case TARGET_NR_fadvise64:
#endif
#ifdef TARGET_S390X
        switch (arg4) {
        case 4: arg4 = POSIX_FADV_NOREUSE + 1; break; /
        case 5: arg4 = POSIX_FADV_NOREUSE + 2; break; /
        case 6: arg4 = POSIX_FADV_DONTNEED; break;
        case 7: arg4 = POSIX_FADV_NOREUSE; break;
        default: break;
        }
#endif
        ret = -posix_fadvise(arg1, arg2, arg3, arg4);
	break;
#endif
#ifdef TARGET_NR_madvise
    case TARGET_NR_madvise:
        /
        ret = get_errno(0);
        break;
#endif
#if TARGET_ABI_BITS == 32
    case TARGET_NR_fcntl64:
    {
	int cmd;
	struct flock64 fl;
	struct target_flock64 *target_fl;
#ifdef TARGET_ARM
	struct target_eabi_flock64 *target_efl;
#endif

	cmd = target_to_host_fcntl_cmd(arg2);
	if (cmd == -TARGET_EINVAL)
		return cmd;

        switch(arg2) {
        case TARGET_F_GETLK64:
#ifdef TARGET_ARM
            if (((CPUARMState *)cpu_env)->eabi) {
                if (!lock_user_struct(VERIFY_READ, target_efl, arg3, 1)) 
                    goto efault;
                fl.l_type = tswap16(target_efl->l_type);
                fl.l_whence = tswap16(target_efl->l_whence);
                fl.l_start = tswap64(target_efl->l_start);
                fl.l_len = tswap64(target_efl->l_len);
                fl.l_pid = tswap32(target_efl->l_pid);
                unlock_user_struct(target_efl, arg3, 0);
            } else
#endif
            {
                if (!lock_user_struct(VERIFY_READ, target_fl, arg3, 1)) 
                    goto efault;
                fl.l_type = tswap16(target_fl->l_type);
                fl.l_whence = tswap16(target_fl->l_whence);
                fl.l_start = tswap64(target_fl->l_start);
                fl.l_len = tswap64(target_fl->l_len);
                fl.l_pid = tswap32(target_fl->l_pid);
                unlock_user_struct(target_fl, arg3, 0);
            }
            ret = get_errno(fcntl(arg1, cmd, &fl));
	    if (ret == 0) {
#ifdef TARGET_ARM
                if (((CPUARMState *)cpu_env)->eabi) {
                    if (!lock_user_struct(VERIFY_WRITE, target_efl, arg3, 0)) 
                        goto efault;
                    target_efl->l_type = tswap16(fl.l_type);
                    target_efl->l_whence = tswap16(fl.l_whence);
                    target_efl->l_start = tswap64(fl.l_start);
                    target_efl->l_len = tswap64(fl.l_len);
                    target_efl->l_pid = tswap32(fl.l_pid);
                    unlock_user_struct(target_efl, arg3, 1);
                } else
#endif
                {
                    if (!lock_user_struct(VERIFY_WRITE, target_fl, arg3, 0)) 
                        goto efault;
                    target_fl->l_type = tswap16(fl.l_type);
                    target_fl->l_whence = tswap16(fl.l_whence);
                    target_fl->l_start = tswap64(fl.l_start);
                    target_fl->l_len = tswap64(fl.l_len);
                    target_fl->l_pid = tswap32(fl.l_pid);
                    unlock_user_struct(target_fl, arg3, 1);
                }
	    }
	    break;

        case TARGET_F_SETLK64:
        case TARGET_F_SETLKW64:
#ifdef TARGET_ARM
            if (((CPUARMState *)cpu_env)->eabi) {
                if (!lock_user_struct(VERIFY_READ, target_efl, arg3, 1)) 
                    goto efault;
                fl.l_type = tswap16(target_efl->l_type);
                fl.l_whence = tswap16(target_efl->l_whence);
                fl.l_start = tswap64(target_efl->l_start);
                fl.l_len = tswap64(target_efl->l_len);
                fl.l_pid = tswap32(target_efl->l_pid);
                unlock_user_struct(target_efl, arg3, 0);
            } else
#endif
            {
                if (!lock_user_struct(VERIFY_READ, target_fl, arg3, 1)) 
                    goto efault;
                fl.l_type = tswap16(target_fl->l_type);
                fl.l_whence = tswap16(target_fl->l_whence);
                fl.l_start = tswap64(target_fl->l_start);
                fl.l_len = tswap64(target_fl->l_len);
                fl.l_pid = tswap32(target_fl->l_pid);
                unlock_user_struct(target_fl, arg3, 0);
            }
            ret = get_errno(fcntl(arg1, cmd, &fl));
	    break;
        default:
            ret = do_fcntl(arg1, arg2, arg3);
            break;
        }
	break;
    }
#endif
#ifdef TARGET_NR_cacheflush
    case TARGET_NR_cacheflush:
        /
        ret = 0;
        break;
#endif
#ifdef TARGET_NR_security
    case TARGET_NR_security:
        goto unimplemented;
#endif
#ifdef TARGET_NR_getpagesize
    case TARGET_NR_getpagesize:
        ret = TARGET_PAGE_SIZE;
        break;
#endif
    case TARGET_NR_gettid:
        ret = get_errno(gettid());
        break;
#ifdef TARGET_NR_readahead
    case TARGET_NR_readahead:
#if TARGET_ABI_BITS == 32
#ifdef TARGET_ARM
        if (((CPUARMState *)cpu_env)->eabi)
        {
            arg2 = arg3;
            arg3 = arg4;
            arg4 = arg5;
        }
#endif
        ret = get_errno(readahead(arg1, ((off64_t)arg3 << 32) | arg2, arg4));
#else
        ret = get_errno(readahead(arg1, arg2, arg3));
#endif
        break;
#endif
#ifdef TARGET_NR_setxattr
    case TARGET_NR_setxattr:
    case TARGET_NR_lsetxattr:
    case TARGET_NR_fsetxattr:
    case TARGET_NR_getxattr:
    case TARGET_NR_lgetxattr:
    case TARGET_NR_fgetxattr:
    case TARGET_NR_listxattr:
    case TARGET_NR_llistxattr:
    case TARGET_NR_flistxattr:
    case TARGET_NR_removexattr:
    case TARGET_NR_lremovexattr:
    case TARGET_NR_fremovexattr:
        ret = -TARGET_EOPNOTSUPP;
        break;
#endif
#ifdef TARGET_NR_set_thread_area
    case TARGET_NR_set_thread_area:
#if defined(TARGET_MIPS)
      ((CPUMIPSState *) cpu_env)->tls_value = arg1;
      ret = 0;
      break;
#elif defined(TARGET_CRIS)
      if (arg1 & 0xff)
          ret = -TARGET_EINVAL;
      else {
          ((CPUCRISState *) cpu_env)->pregs[PR_PID] = arg1;
          ret = 0;
      }
      break;
#elif defined(TARGET_I386) && defined(TARGET_ABI32)
      ret = do_set_thread_area(cpu_env, arg1);
      break;
#else
      goto unimplemented_nowarn;
#endif
#endif
#ifdef TARGET_NR_get_thread_area
    case TARGET_NR_get_thread_area:
#if defined(TARGET_I386) && defined(TARGET_ABI32)
        ret = do_get_thread_area(cpu_env, arg1);
#else
        goto unimplemented_nowarn;
#endif
#endif
#ifdef TARGET_NR_getdomainname
    case TARGET_NR_getdomainname:
        goto unimplemented_nowarn;
#endif

#ifdef TARGET_NR_clock_gettime
    case TARGET_NR_clock_gettime:
    {
        struct timespec ts;
        ret = get_errno(clock_gettime(arg1, &ts));
        if (!is_error(ret)) {
            host_to_target_timespec(arg2, &ts);
        }
        break;
    }
#endif
#ifdef TARGET_NR_clock_getres
    case TARGET_NR_clock_getres:
    {
        struct timespec ts;
        ret = get_errno(clock_getres(arg1, &ts));
        if (!is_error(ret)) {
            host_to_target_timespec(arg2, &ts);
        }
        break;
    }
#endif
#ifdef TARGET_NR_clock_nanosleep
    case TARGET_NR_clock_nanosleep:
    {
        struct timespec ts;
        target_to_host_timespec(&ts, arg3);
        ret = get_errno(clock_nanosleep(arg1, arg2, &ts, arg4 ? &ts : NULL));
        if (arg4)
            host_to_target_timespec(arg4, &ts);
        break;
    }
#endif

#if defined(TARGET_NR_set_tid_address) && defined(__NR_set_tid_address)
    case TARGET_NR_set_tid_address:
        ret = get_errno(set_tid_address((int *)g2h(arg1)));
        break;
#endif

#if defined(TARGET_NR_tkill) && defined(__NR_tkill)
    case TARGET_NR_tkill:
        ret = get_errno(sys_tkill((int)arg1, target_to_host_signal(arg2)));
        break;
#endif

#if defined(TARGET_NR_tgkill) && defined(__NR_tgkill)
    case TARGET_NR_tgkill:
	ret = get_errno(sys_tgkill((int)arg1, (int)arg2,
                        target_to_host_signal(arg3)));
	break;
#endif

#ifdef TARGET_NR_set_robust_list
    case TARGET_NR_set_robust_list:
	goto unimplemented_nowarn;
#endif

#if defined(TARGET_NR_utimensat) && defined(__NR_utimensat)
    case TARGET_NR_utimensat:
        {
            struct timespec *tsp, ts[2];
            if (!arg3) {
                tsp = NULL;
            } else {
                target_to_host_timespec(ts, arg3);
                target_to_host_timespec(ts+1, arg3+sizeof(struct target_timespec));
                tsp = ts;
            }
            if (!arg2)
                ret = get_errno(sys_utimensat(arg1, NULL, tsp, arg4));
            else {
                if (!(p = lock_user_string(arg2))) {
                    ret = -TARGET_EFAULT;
                    goto fail;
                }
                ret = get_errno(sys_utimensat(arg1, path(p), tsp, arg4));
                unlock_user(p, arg2, 0);
            }
        }
	break;
#endif
#if defined(CONFIG_USE_NPTL)
    case TARGET_NR_futex:
        ret = do_futex(arg1, arg2, arg3, arg4, arg5, arg6);
        break;
#endif
#if defined(TARGET_NR_inotify_init) && defined(__NR_inotify_init)
    case TARGET_NR_inotify_init:
        ret = get_errno(sys_inotify_init());
        break;
#endif
#ifdef CONFIG_INOTIFY1
#if defined(TARGET_NR_inotify_init1) && defined(__NR_inotify_init1)
    case TARGET_NR_inotify_init1:
        ret = get_errno(sys_inotify_init1(arg1));
        break;
#endif
#endif
#if defined(TARGET_NR_inotify_add_watch) && defined(__NR_inotify_add_watch)
    case TARGET_NR_inotify_add_watch:
        p = lock_user_string(arg2);
        ret = get_errno(sys_inotify_add_watch(arg1, path(p), arg3));
        unlock_user(p, arg2, 0);
        break;
#endif
#if defined(TARGET_NR_inotify_rm_watch) && defined(__NR_inotify_rm_watch)
    case TARGET_NR_inotify_rm_watch:
        ret = get_errno(sys_inotify_rm_watch(arg1, arg2));
        break;
#endif

#if defined(TARGET_NR_mq_open) && defined(__NR_mq_open)
    case TARGET_NR_mq_open:
        {
            struct mq_attr posix_mq_attr;

            p = lock_user_string(arg1 - 1);
            if (arg4 != 0)
                copy_from_user_mq_attr (&posix_mq_attr, arg4);
            ret = get_errno(mq_open(p, arg2, arg3, &posix_mq_attr));
            unlock_user (p, arg1, 0);
        }
        break;

    case TARGET_NR_mq_unlink:
        p = lock_user_string(arg1 - 1);
        ret = get_errno(mq_unlink(p));
        unlock_user (p, arg1, 0);
        break;

    case TARGET_NR_mq_timedsend:
        {
            struct timespec ts;

            p = lock_user (VERIFY_READ, arg2, arg3, 1);
            if (arg5 != 0) {
                target_to_host_timespec(&ts, arg5);
                ret = get_errno(mq_timedsend(arg1, p, arg3, arg4, &ts));
                host_to_target_timespec(arg5, &ts);
            }
            else
                ret = get_errno(mq_send(arg1, p, arg3, arg4));
            unlock_user (p, arg2, arg3);
        }
        break;

    case TARGET_NR_mq_timedreceive:
        {
            struct timespec ts;
            unsigned int prio;

            p = lock_user (VERIFY_READ, arg2, arg3, 1);
            if (arg5 != 0) {
                target_to_host_timespec(&ts, arg5);
                ret = get_errno(mq_timedreceive(arg1, p, arg3, &prio, &ts));
                host_to_target_timespec(arg5, &ts);
            }
            else
                ret = get_errno(mq_receive(arg1, p, arg3, &prio));
            unlock_user (p, arg2, arg3);
            if (arg4 != 0)
                put_user_u32(prio, arg4);
        }
        break;

    /
/
/

    case TARGET_NR_mq_getsetattr:
        {
            struct mq_attr posix_mq_attr_in, posix_mq_attr_out;
            ret = 0;
            if (arg3 != 0) {
                ret = mq_getattr(arg1, &posix_mq_attr_out);
                copy_to_user_mq_attr(arg3, &posix_mq_attr_out);
            }
            if (arg2 != 0) {
                copy_from_user_mq_attr(&posix_mq_attr_in, arg2);
                ret |= mq_setattr(arg1, &posix_mq_attr_in, &posix_mq_attr_out);
            }

        }
        break;
#endif

#ifdef CONFIG_SPLICE
#ifdef TARGET_NR_tee
    case TARGET_NR_tee:
        {
            ret = get_errno(tee(arg1,arg2,arg3,arg4));
        }
        break;
#endif
#ifdef TARGET_NR_splice
    case TARGET_NR_splice:
        {
            loff_t loff_in, loff_out;
            loff_t *ploff_in = NULL, *ploff_out = NULL;
            if(arg2) {
                get_user_u64(loff_in, arg2);
                ploff_in = &loff_in;
            }
            if(arg4) {
                get_user_u64(loff_out, arg2);
                ploff_out = &loff_out;
            }
            ret = get_errno(splice(arg1, ploff_in, arg3, ploff_out, arg5, arg6));
        }
        break;
#endif
#ifdef TARGET_NR_vmsplice
	case TARGET_NR_vmsplice:
        {
            int count = arg3;
            struct iovec *vec;

            vec = alloca(count * sizeof(struct iovec));
            if (lock_iovec(VERIFY_READ, vec, arg2, count, 1) < 0)
                goto efault;
            ret = get_errno(vmsplice(arg1, vec, count, arg4));
            unlock_iovec(vec, arg2, count, 0);
        }
        break;
#endif
#endif /
#ifdef CONFIG_EVENTFD
#if defined(TARGET_NR_eventfd)
    case TARGET_NR_eventfd:
        ret = get_errno(eventfd(arg1, 0));
        break;
#endif
#if defined(TARGET_NR_eventfd2)
    case TARGET_NR_eventfd2:
        ret = get_errno(eventfd(arg1, arg2));
        break;
#endif
#endif /
#if defined(CONFIG_FALLOCATE) && defined(TARGET_NR_fallocate)
    case TARGET_NR_fallocate:
        ret = get_errno(fallocate(arg1, arg2, arg3, arg4));
        break;
#endif
#if defined(CONFIG_SYNC_FILE_RANGE)
#if defined(TARGET_NR_sync_file_range)
    case TARGET_NR_sync_file_range:
#if TARGET_ABI_BITS == 32
        ret = get_errno(sync_file_range(arg1, target_offset64(arg2, arg3),
                                        target_offset64(arg4, arg5), arg6));
#else
        ret = get_errno(sync_file_range(arg1, arg2, arg3, arg4));
#endif
        break;
#endif
#if defined(TARGET_NR_sync_file_range2)
    case TARGET_NR_sync_file_range2:
        /
#if TARGET_ABI_BITS == 32
        ret = get_errno(sync_file_range(arg1, target_offset64(arg3, arg4),
                                        target_offset64(arg5, arg6), arg2));
#else
        ret = get_errno(sync_file_range(arg1, arg3, arg4, arg2));
#endif
        break;
#endif
#endif
#if defined(CONFIG_EPOLL)
#if defined(TARGET_NR_epoll_create)
    case TARGET_NR_epoll_create:
        ret = get_errno(epoll_create(arg1));
        break;
#endif
#if defined(TARGET_NR_epoll_create1) && defined(CONFIG_EPOLL_CREATE1)
    case TARGET_NR_epoll_create1:
        ret = get_errno(epoll_create1(arg1));
        break;
#endif
#if defined(TARGET_NR_epoll_ctl)
    case TARGET_NR_epoll_ctl:
    {
        struct epoll_event ep;
        struct epoll_event *epp = 0;
        if (arg4) {
            struct target_epoll_event *target_ep;
            if (!lock_user_struct(VERIFY_READ, target_ep, arg4, 1)) {
                goto efault;
            }
            ep.events = tswap32(target_ep->events);
            /
            ep.data.u64 = tswap64(target_ep->data.u64);
            unlock_user_struct(target_ep, arg4, 0);
            epp = &ep;
        }
        ret = get_errno(epoll_ctl(arg1, arg2, arg3, epp));
        break;
    }
#endif

#if defined(TARGET_NR_epoll_pwait) && defined(CONFIG_EPOLL_PWAIT)
#define IMPLEMENT_EPOLL_PWAIT
#endif
#if defined(TARGET_NR_epoll_wait) || defined(IMPLEMENT_EPOLL_PWAIT)
#if defined(TARGET_NR_epoll_wait)
    case TARGET_NR_epoll_wait:
#endif
#if defined(IMPLEMENT_EPOLL_PWAIT)
    case TARGET_NR_epoll_pwait:
#endif
    {
        struct target_epoll_event *target_ep;
        struct epoll_event *ep;
        int epfd = arg1;
        int maxevents = arg3;
        int timeout = arg4;

        target_ep = lock_user(VERIFY_WRITE, arg2,
                              maxevents * sizeof(struct target_epoll_event), 1);
        if (!target_ep) {
            goto efault;
        }

        ep = alloca(maxevents * sizeof(struct epoll_event));

        switch (num) {
#if defined(IMPLEMENT_EPOLL_PWAIT)
        case TARGET_NR_epoll_pwait:
        {
            target_sigset_t *target_set;
            sigset_t _set, *set = &_set;

            if (arg5) {
                target_set = lock_user(VERIFY_READ, arg5,
                                       sizeof(target_sigset_t), 1);
                if (!target_set) {
                    unlock_user(target_ep, arg2, 0);
                    goto efault;
                }
                target_to_host_sigset(set, target_set);
                unlock_user(target_set, arg5, 0);
            } else {
                set = NULL;
            }

            ret = get_errno(epoll_pwait(epfd, ep, maxevents, timeout, set));
            break;
        }
#endif
#if defined(TARGET_NR_epoll_wait)
        case TARGET_NR_epoll_wait:
            ret = get_errno(epoll_wait(epfd, ep, maxevents, timeout));
            break;
#endif
        default:
            ret = -TARGET_ENOSYS;
        }
        if (!is_error(ret)) {
            int i;
            for (i = 0; i < ret; i++) {
                target_ep[i].events = tswap32(ep[i].events);
                target_ep[i].data.u64 = tswap64(ep[i].data.u64);
            }
        }
        unlock_user(target_ep, arg2, ret * sizeof(struct target_epoll_event));
        break;
    }
#endif
#endif
    default:
    unimplemented:
        gemu_log("qemu: Unsupported syscall: %d\n", num);
#if defined(TARGET_NR_setxattr) || defined(TARGET_NR_get_thread_area) || defined(TARGET_NR_getdomainname) || defined(TARGET_NR_set_robust_list)
    unimplemented_nowarn:
#endif
        ret = -TARGET_ENOSYS;
        break;
    }
fail:
#ifdef DEBUG
    gemu_log(" = " TARGET_ABI_FMT_ld "\n", ret);
#endif
    if(do_strace)
        print_syscall_ret(num, ret);
    return ret;
efault:
    ret = -TARGET_EFAULT;
    goto fail;
}
