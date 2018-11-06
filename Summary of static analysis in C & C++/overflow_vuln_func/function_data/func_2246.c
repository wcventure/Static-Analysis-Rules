target_ulong do_load_msr (CPUPPCState *env)
{
    return
#if defined (TARGET_PPC64)
        (msr_sf   << MSR_SF)   |
        (msr_isf  << MSR_ISF)  |
        (msr_hv   << MSR_HV)   |
#endif
        (msr_ucle << MSR_UCLE) |
        (msr_vr   << MSR_VR)   | /
        (msr_ap   << MSR_AP)   |
        (msr_sa   << MSR_SA)   |
        (msr_key  << MSR_KEY)  |
        (msr_pow  << MSR_POW)  | /
        (msr_tlb  << MSR_TLB)  | /
        (msr_ile  << MSR_ILE)  |
        (msr_ee   << MSR_EE)   |
        (msr_pr   << MSR_PR)   |
        (msr_fp   << MSR_FP)   |
        (msr_me   << MSR_ME)   |
        (msr_fe0  << MSR_FE0)  |
        (msr_se   << MSR_SE)   | /
        (msr_be   << MSR_BE)   | /
        (msr_fe1  << MSR_FE1)  |
        (msr_al   << MSR_AL)   |
        (msr_ip   << MSR_IP)   |
        (msr_ir   << MSR_IR)   | /
        (msr_dr   << MSR_DR)   | /
        (msr_pe   << MSR_PE)   | /
        (msr_px   << MSR_PX)   | /
        (msr_ri   << MSR_RI)   |
        (msr_le   << MSR_LE);
}
