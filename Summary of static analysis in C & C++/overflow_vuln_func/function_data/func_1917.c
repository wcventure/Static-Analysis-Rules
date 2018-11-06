static void ppc405ep_compute_clocks (ppc405ep_cpc_t *cpc)
{
    uint32_t CPU_clk, PLB_clk, OPB_clk, EBC_clk, MAL_clk, PCI_clk;
    uint32_t UART0_clk, UART1_clk;
    uint64_t VCO_out, PLL_out;
    int M, D;

    VCO_out = 0;
    if ((cpc->pllmr[1] & 0x80000000) && !(cpc->pllmr[1] & 0x40000000)) {
        M = (((cpc->pllmr[1] >> 20) - 1) & 0xF) + 1; /
#ifdef DEBUG_CLOCKS_LL
        printf("FBMUL %01" PRIx32 " %d\n", (cpc->pllmr[1] >> 20) & 0xF, M);
#endif
        D = 8 - ((cpc->pllmr[1] >> 16) & 0x7); /
#ifdef DEBUG_CLOCKS_LL
        printf("FWDA %01" PRIx32 " %d\n", (cpc->pllmr[1] >> 16) & 0x7, D);
#endif
        VCO_out = cpc->sysclk * M * D;
        if (VCO_out < 500000000UL || VCO_out > 1000000000UL) {
            /
            printf("VCO out of range %" PRIu64 "\n", VCO_out);
#if 0
            cpc->pllmr[1] &= ~0x80000000;
            goto pll_bypass;
#endif
        }
        PLL_out = VCO_out / D;
        /
        cpc->boot |= 0x00000001;
    } else {
#if 0
    pll_bypass:
#endif
        PLL_out = cpc->sysclk;
        if (cpc->pllmr[1] & 0x40000000) {
            /
            cpc->boot &= ~0x00000001;
        }
    }
    /
    D = ((cpc->pllmr[0] >> 20) & 0x3) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("CCDV %01" PRIx32 " %d\n", (cpc->pllmr[0] >> 20) & 0x3, D);
#endif
    CPU_clk = PLL_out / D;
    D = ((cpc->pllmr[0] >> 16) & 0x3) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("CBDV %01" PRIx32 " %d\n", (cpc->pllmr[0] >> 16) & 0x3, D);
#endif
    PLB_clk = CPU_clk / D;
    D = ((cpc->pllmr[0] >> 12) & 0x3) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("OPDV %01" PRIx32 " %d\n", (cpc->pllmr[0] >> 12) & 0x3, D);
#endif
    OPB_clk = PLB_clk / D;
    D = ((cpc->pllmr[0] >> 8) & 0x3) + 2; /
#ifdef DEBUG_CLOCKS_LL
    printf("EPDV %01" PRIx32 " %d\n", (cpc->pllmr[0] >> 8) & 0x3, D);
#endif
    EBC_clk = PLB_clk / D;
    D = ((cpc->pllmr[0] >> 4) & 0x3) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("MPDV %01" PRIx32 " %d\n", (cpc->pllmr[0] >> 4) & 0x3, D);
#endif
    MAL_clk = PLB_clk / D;
    D = (cpc->pllmr[0] & 0x3) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("PPDV %01" PRIx32 " %d\n", cpc->pllmr[0] & 0x3, D);
#endif
    PCI_clk = PLB_clk / D;
    D = ((cpc->ucr - 1) & 0x7F) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("U0DIV %01" PRIx32 " %d\n", cpc->ucr & 0x7F, D);
#endif
    UART0_clk = PLL_out / D;
    D = (((cpc->ucr >> 8) - 1) & 0x7F) + 1; /
#ifdef DEBUG_CLOCKS_LL
    printf("U1DIV %01" PRIx32 " %d\n", (cpc->ucr >> 8) & 0x7F, D);
#endif
    UART1_clk = PLL_out / D;
#ifdef DEBUG_CLOCKS
    printf("Setup PPC405EP clocks - sysclk %" PRIu32 " VCO %" PRIu64
           " PLL out %" PRIu64 " Hz\n", cpc->sysclk, VCO_out, PLL_out);
    printf("CPU %" PRIu32 " PLB %" PRIu32 " OPB %" PRIu32 " EBC %" PRIu32
           " MAL %" PRIu32 " PCI %" PRIu32 " UART0 %" PRIu32
           " UART1 %" PRIu32 "\n",
           CPU_clk, PLB_clk, OPB_clk, EBC_clk, MAL_clk, PCI_clk,
           UART0_clk, UART1_clk);
#endif
    /
    clk_setup(&cpc->clk_setup[PPC405EP_CPU_CLK], CPU_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_PLB_CLK], PLB_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_OPB_CLK], OPB_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_EBC_CLK], EBC_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_MAL_CLK], MAL_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_PCI_CLK], PCI_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_UART0_CLK], UART0_clk);
    /
    clk_setup(&cpc->clk_setup[PPC405EP_UART1_CLK], UART1_clk);
}
