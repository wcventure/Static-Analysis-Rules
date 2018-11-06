static void tcg_out_tlb_check (TCGContext *s, int r0, int r1, int r2,
                               int addr_reg, int addr_reg2, int s_bits,
                               int offset1, int offset2, uint8_t **label_ptr)
{
    uint16_t retranst;

    tcg_out32 (s, (RLWINM
                   | RA (r0)
                   | RS (addr_reg)
                   | SH (32 - (TARGET_PAGE_BITS - CPU_TLB_ENTRY_BITS))
                   | MB (32 - (CPU_TLB_BITS + CPU_TLB_ENTRY_BITS))
                   | ME (31 - CPU_TLB_ENTRY_BITS)
                   )
        );
    tcg_out32 (s, ADD | RT (r0) | RA (r0) | RB (TCG_AREG0));
    tcg_out32 (s, (LWZU
                   | RT (r1)
                   | RA (r0)
                   | offset1
                   )
        );
    tcg_out32 (s, (RLWINM
                   | RA (r2)
                   | RS (addr_reg)
                   | SH (0)
                   | MB ((32 - s_bits) & 31)
                   | ME (31 - TARGET_PAGE_BITS)
                   )
        );

    tcg_out32 (s, CMP | BF (7) | RA (r2) | RB (r1));
#if TARGET_LONG_BITS == 64
    tcg_out32 (s, LWZ | RT (r1) | RA (r0) | 4);
    tcg_out32 (s, CMP | BF (6) | RA (addr_reg2) | RB (r1));
    tcg_out32 (s, CRAND | BT (7, CR_EQ) | BA (6, CR_EQ) | BB (7, CR_EQ));
#endif

    /
    *label_ptr = s->code_ptr;
    retranst = ((uint16_t *) s->code_ptr)[1] & ~3;
    tcg_out32(s, BC | BI(7, CR_EQ) | retranst | BO_COND_FALSE | LK);

    /
    tcg_out32 (s, (LWZ
                   | RT (r0)
                   | RA (r0)
                   | offset2
                   )
        );
    /
    tcg_out32 (s, ADD | RT (r0) | RA (r0) | RB (addr_reg));
    /

}
