void do_compute_hflags (CPUPPCState *env)
{
    /
    env->hflags = (msr_pr << MSR_PR) | (msr_le << MSR_LE) |
        (msr_fp << MSR_FP) | (msr_fe0 << MSR_FE0) | (msr_fe1 << MSR_FE1) |
        (msr_vr << MSR_VR) | (msr_ap << MSR_AP) | (msr_sa << MSR_SA) | 
        (msr_se << MSR_SE) | (msr_be << MSR_BE);
#if defined (TARGET_PPC64)
    env->hflags |= (msr_sf << MSR_SF) | (msr_hv << MSR_HV);
#endif
}
