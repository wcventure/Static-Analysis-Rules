void do_4xx_tlbsx_ (void)
{
    int tmp = xer_ov;

    T0 = ppcemb_tlb_search(env, T0, env->spr[SPR_40x_PID]);
    if (T0 != -1)
        tmp |= 0x02;
    env->crf[0] = tmp;
}
