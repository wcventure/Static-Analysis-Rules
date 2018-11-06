static void inner_add_yblock_bw_16_obmc_32_mmx(const uint8_t *obmc, const long obmc_stride, uint8_t * * block, int b_w, long b_h,
                      int src_x, int src_y, long src_stride, slice_buffer * sb, int add, uint8_t * dst8){
snow_inner_add_yblock_mmx_header
snow_inner_add_yblock_mmx_start("mm1", "mm5", "3", "0", "0")
snow_inner_add_yblock_mmx_accum("2", "16", "0")
snow_inner_add_yblock_mmx_accum("1", "512", "0")
snow_inner_add_yblock_mmx_accum("0", "528", "0")
snow_inner_add_yblock_mmx_mix("0", "0")

snow_inner_add_yblock_mmx_start("mm1", "mm5", "3", "8", "8")
snow_inner_add_yblock_mmx_accum("2", "24", "8")
snow_inner_add_yblock_mmx_accum("1", "520", "8")
snow_inner_add_yblock_mmx_accum("0", "536", "8")
snow_inner_add_yblock_mmx_mix("32", "8")
snow_inner_add_yblock_mmx_end("32")
}
