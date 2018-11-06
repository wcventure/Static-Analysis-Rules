av_cold void ff_vp9dsp_init(VP9DSPContext *dsp, int bpp)
{
    if (bpp == 8) {
        ff_vp9dsp_init_8(dsp);
    } else if (bpp == 10) {
        ff_vp9dsp_init_10(dsp);
    } else {
        av_assert0(bpp == 12);
        ff_vp9dsp_init_12(dsp);
    }

    if (ARCH_X86) ff_vp9dsp_init_x86(dsp, bpp);
    if (ARCH_MIPS) ff_vp9dsp_init_mips(dsp, bpp);
}
