static int kvm_put_msrs(X86CPU *cpu, int level)
{
    CPUX86State *env = &cpu->env;
    struct {
        struct kvm_msrs info;
        struct kvm_msr_entry entries[100];
    } msr_data;
    struct kvm_msr_entry *msrs = msr_data.entries;
    int n = 0, i;

    kvm_msr_entry_set(&msrs[n++], MSR_IA32_SYSENTER_CS, env->sysenter_cs);
    kvm_msr_entry_set(&msrs[n++], MSR_IA32_SYSENTER_ESP, env->sysenter_esp);
    kvm_msr_entry_set(&msrs[n++], MSR_IA32_SYSENTER_EIP, env->sysenter_eip);
    kvm_msr_entry_set(&msrs[n++], MSR_PAT, env->pat);
    if (has_msr_star) {
        kvm_msr_entry_set(&msrs[n++], MSR_STAR, env->star);
    }
    if (has_msr_hsave_pa) {
        kvm_msr_entry_set(&msrs[n++], MSR_VM_HSAVE_PA, env->vm_hsave);
    }
    if (has_msr_tsc_adjust) {
        kvm_msr_entry_set(&msrs[n++], MSR_TSC_ADJUST, env->tsc_adjust);
    }
    if (has_msr_misc_enable) {
        kvm_msr_entry_set(&msrs[n++], MSR_IA32_MISC_ENABLE,
                          env->msr_ia32_misc_enable);
    }
#ifdef TARGET_X86_64
    if (lm_capable_kernel) {
        kvm_msr_entry_set(&msrs[n++], MSR_CSTAR, env->cstar);
        kvm_msr_entry_set(&msrs[n++], MSR_KERNELGSBASE, env->kernelgsbase);
        kvm_msr_entry_set(&msrs[n++], MSR_FMASK, env->fmask);
        kvm_msr_entry_set(&msrs[n++], MSR_LSTAR, env->lstar);
    }
#endif
    if (level == KVM_PUT_FULL_STATE) {
        kvm_msr_entry_set(&msrs[n++], MSR_IA32_TSC, env->tsc);
    }
    /
    if (level >= KVM_PUT_RESET_STATE) {
        kvm_msr_entry_set(&msrs[n++], MSR_KVM_SYSTEM_TIME,
                          env->system_time_msr);
        kvm_msr_entry_set(&msrs[n++], MSR_KVM_WALL_CLOCK, env->wall_clock_msr);
        if (has_msr_async_pf_en) {
            kvm_msr_entry_set(&msrs[n++], MSR_KVM_ASYNC_PF_EN,
                              env->async_pf_en_msr);
        }
        if (has_msr_pv_eoi_en) {
            kvm_msr_entry_set(&msrs[n++], MSR_KVM_PV_EOI_EN,
                              env->pv_eoi_en_msr);
        }
        if (has_msr_kvm_steal_time) {
            kvm_msr_entry_set(&msrs[n++], MSR_KVM_STEAL_TIME,
                              env->steal_time_msr);
        }
        if (has_msr_architectural_pmu) {
            /
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_FIXED_CTR_CTRL, 0);
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_GLOBAL_CTRL, 0);

            /
            for (i = 0; i < MAX_FIXED_COUNTERS; i++) {
                kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_FIXED_CTR0 + i,
                                  env->msr_fixed_counters[i]);
            }
            for (i = 0; i < num_architectural_pmu_counters; i++) {
                kvm_msr_entry_set(&msrs[n++], MSR_P6_PERFCTR0 + i,
                                  env->msr_gp_counters[i]);
                kvm_msr_entry_set(&msrs[n++], MSR_P6_EVNTSEL0 + i,
                                  env->msr_gp_evtsel[i]);
            }
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_GLOBAL_STATUS,
                              env->msr_global_status);
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_GLOBAL_OVF_CTRL,
                              env->msr_global_ovf_ctrl);

            /
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_FIXED_CTR_CTRL,
                              env->msr_fixed_ctr_ctrl);
            kvm_msr_entry_set(&msrs[n++], MSR_CORE_PERF_GLOBAL_CTRL,
                              env->msr_global_ctrl);
        }
        if (hyperv_hypercall_available(cpu)) {
            kvm_msr_entry_set(&msrs[n++], HV_X64_MSR_GUEST_OS_ID, 0);
            kvm_msr_entry_set(&msrs[n++], HV_X64_MSR_HYPERCALL, 0);
        }
        if (cpu->hyperv_vapic) {
            kvm_msr_entry_set(&msrs[n++], HV_X64_MSR_APIC_ASSIST_PAGE, 0);
        }
        if (has_msr_feature_control) {
            kvm_msr_entry_set(&msrs[n++], MSR_IA32_FEATURE_CONTROL,
                              env->msr_ia32_feature_control);
        }
        if (has_msr_bndcfgs) {
            kvm_msr_entry_set(&msrs[n++], MSR_IA32_BNDCFGS, env->msr_bndcfgs);
        }
    }
    if (env->mcg_cap) {
        int i;

        kvm_msr_entry_set(&msrs[n++], MSR_MCG_STATUS, env->mcg_status);
        kvm_msr_entry_set(&msrs[n++], MSR_MCG_CTL, env->mcg_ctl);
        for (i = 0; i < (env->mcg_cap & 0xff) * 4; i++) {
            kvm_msr_entry_set(&msrs[n++], MSR_MC0_CTL + i, env->mce_banks[i]);
        }
    }

    msr_data.info.nmsrs = n;

    return kvm_vcpu_ioctl(CPU(cpu), KVM_SET_MSRS, &msr_data);

}
