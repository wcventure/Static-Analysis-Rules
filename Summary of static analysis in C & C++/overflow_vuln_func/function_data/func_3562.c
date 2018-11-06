av_cold void ff_idctdsp_init_x86(IDCTDSPContext *c, AVCodecContext *avctx,
                                 unsigned high_bit_depth)
{
    int cpu_flags = av_get_cpu_flags();

    if (INLINE_MMX(cpu_flags)) {
        if (!high_bit_depth &&
            avctx->lowres == 0 &&
            (avctx->idct_algo == FF_IDCT_AUTO ||
             avctx->idct_algo == FF_IDCT_SIMPLEAUTO ||
             avctx->idct_algo == FF_IDCT_SIMPLEMMX)) {
                c->idct_put  = ff_simple_idct_put_mmx;
                c->idct_add  = ff_simple_idct_add_mmx;
                c->idct      = ff_simple_idct_mmx;
                c->perm_type = FF_IDCT_PERM_SIMPLE;
        }
    }
    if (EXTERNAL_MMX(cpu_flags)) {
        c->put_signed_pixels_clamped = ff_put_signed_pixels_clamped_mmx;
        c->put_pixels_clamped        = ff_put_pixels_clamped_mmx;
        c->add_pixels_clamped        = ff_add_pixels_clamped_mmx;
    }
    if (EXTERNAL_SSE2(cpu_flags)) {
        c->put_signed_pixels_clamped = ff_put_signed_pixels_clamped_sse2;
        c->put_pixels_clamped        = ff_put_pixels_clamped_sse2;
        c->add_pixels_clamped        = ff_add_pixels_clamped_sse2;
    }

    if (ARCH_X86_64 &&
        avctx->bits_per_raw_sample == 10 && avctx->lowres == 0 &&
        (avctx->idct_algo == FF_IDCT_AUTO ||
         avctx->idct_algo == FF_IDCT_SIMPLEAUTO ||
         avctx->idct_algo == FF_IDCT_SIMPLE)) {
        if (EXTERNAL_SSE2(cpu_flags)) {
            c->idct_put  = ff_simple_idct10_put_sse2;
            c->idct_add  = NULL;
            c->idct      = ff_simple_idct10_sse2;
            c->perm_type = FF_IDCT_PERM_TRANSPOSE;

        }
        if (EXTERNAL_AVX(cpu_flags)) {
            c->idct_put  = ff_simple_idct10_put_avx;
            c->idct_add  = NULL;
            c->idct      = ff_simple_idct10_avx;
            c->perm_type = FF_IDCT_PERM_TRANSPOSE;
        }
    }
}
