static inline void fimd_swap_data(unsigned int swap_ctl, uint64_t *data)
{
    int i;
    uint64_t res;
    uint64_t x = *data;

    if (swap_ctl & FIMD_WINCON_SWAP_BITS) {
        res = 0;
        for (i = 0; i < 64; i++) {
            if (x & (1ULL << (64 - i))) {
                res |= (1ULL << i);
            }
        }
        x = res;
    }

    if (swap_ctl & FIMD_WINCON_SWAP_BYTE) {
        x = bswap64(x);
    }

    if (swap_ctl & FIMD_WINCON_SWAP_HWORD) {
        x = ((x & 0x000000000000FFFFULL) << 48) |
            ((x & 0x00000000FFFF0000ULL) << 16) |
            ((x & 0x0000FFFF00000000ULL) >> 16) |
            ((x & 0xFFFF000000000000ULL) >> 48);
    }

    if (swap_ctl & FIMD_WINCON_SWAP_WORD) {
        x = ((x & 0x00000000FFFFFFFFULL) << 32) |
            ((x & 0xFFFFFFFF00000000ULL) >> 32);
    }

    *data = x;
}
