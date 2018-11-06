static void spr_read_xer (DisasContext *ctx, int gprn, int sprn)
{
    gen_read_xer(cpu_gpr[gprn]);
}
