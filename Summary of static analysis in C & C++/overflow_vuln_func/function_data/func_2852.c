void mulu64(uint64_t *phigh, uint64_t *plow, uint64_t a, uint64_t b)
{
#if defined(__x86_64__)
    __asm__ ("mul %0\n\t"
             : "=d" (*phigh), "=a" (*plow)
             : "a" (a), "0" (b)
            );
#else
    uint64_t ph, pm1, pm2, pl;

    pl = (uint64_t)((uint32_t)a) * (uint64_t)((uint32_t)b);
    pm1 = (a >> 32) * (uint32_t)b;
    pm2 = (uint32_t)a * (b >> 32);
    ph = (a >> 32) * (b >> 32);

    ph += pm1 >> 32;
    pm1 = (uint64_t)((uint32_t)pm1) + pm2 + (pl >> 32);

    *phigh = ph + (pm1 >> 32);
    *plow = (pm1 << 32) + (uint32_t)pl;
#endif
}
