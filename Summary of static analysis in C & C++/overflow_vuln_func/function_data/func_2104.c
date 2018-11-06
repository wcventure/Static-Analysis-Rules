#if defined(TARGET_PPC64)
void OPPROTO op_subfme_64 (void)
{
    T0 = ~T0 + xer_ca - 1;
    if (likely((uint64_t)T0 != (uint64_t)-1))
        xer_ca = 1;
    RETURN();
}
