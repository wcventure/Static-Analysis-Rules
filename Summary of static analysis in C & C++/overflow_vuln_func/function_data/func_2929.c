static uint32_t suov32(CPUTriCoreState *env, int64_t arg)
{
    uint32_t ret;
    int64_t max_pos = UINT32_MAX;
    if (arg > max_pos) {
        env->PSW_USB_V = (1 << 31);
        env->PSW_USB_SV = (1 << 31);
        ret = (target_ulong)max_pos;
    } else {
        if (arg < 0) {
            env->PSW_USB_V = (1 << 31);
            env->PSW_USB_SV = (1 << 31);
            ret = 0;
        } else {
            env->PSW_USB_V = 0;
            ret = (target_ulong)arg;
        }
     }
    env->PSW_USB_AV = arg ^ arg * 2u;
    env->PSW_USB_SAV |= env->PSW_USB_AV;
    return ret;
}
