#if defined(TARGET_PPC64)
void do_mulldo (void)
{
    int64_t th;
    uint64_t tl;

    muls64(&tl, &th, T0, T1);
    if (likely(th == 0)) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
    T0 = (int64_t)tl;
}
