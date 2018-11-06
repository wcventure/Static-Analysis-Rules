static int init_duplicate_context(MpegEncContext *s, MpegEncContext *base)
{
    int y_size = s->b8_stride * (2 * s->mb_height + 1);
    int c_size = s->mb_stride * (s->mb_height + 1);
    int yc_size = y_size + 2 * c_size;
    int i;

    // edge emu needs blocksize + filter length - 1
    // (= 17x17 for  halfpel / 21x21 for  h264)
    FF_ALLOCZ_OR_GOTO(s->avctx, s->edge_emu_buffer,
                      (s->width + 64) * 2 * 21 * 2, fail);    // (width + edge + align)*interlaced*MBsize*tolerance

    // FIXME should be linesize instead of s->width * 2
    // but that is not known before get_buffer()
    FF_ALLOCZ_OR_GOTO(s->avctx, s->me.scratchpad,
                      (s->width + 64) * 4 * 16 * 2 * sizeof(uint8_t), fail)
    s->me.temp         = s->me.scratchpad;
    s->rd_scratchpad   = s->me.scratchpad;
    s->b_scratchpad    = s->me.scratchpad;
    s->obmc_scratchpad = s->me.scratchpad + 16;
    if (s->encoding) {
        FF_ALLOCZ_OR_GOTO(s->avctx, s->me.map,
                          ME_MAP_SIZE * sizeof(uint32_t), fail)
        FF_ALLOCZ_OR_GOTO(s->avctx, s->me.score_map,
                          ME_MAP_SIZE * sizeof(uint32_t), fail)
        if (s->avctx->noise_reduction) {
            FF_ALLOCZ_OR_GOTO(s->avctx, s->dct_error_sum,
                              2 * 64 * sizeof(int), fail)
        }
    }
    FF_ALLOCZ_OR_GOTO(s->avctx, s->blocks, 64 * 12 * 2 * sizeof(DCTELEM), fail)
    s->block = s->blocks[0];

    for (i = 0; i < 12; i++) {
        s->pblocks[i] = &s->block[i];
    }

    if (s->out_format == FMT_H263) {
        /
        FF_ALLOCZ_OR_GOTO(s->avctx, s->ac_val_base,
                          yc_size * sizeof(int16_t) * 16, fail);
        s->ac_val[0] = s->ac_val_base + s->b8_stride + 1;
        s->ac_val[1] = s->ac_val_base + y_size + s->mb_stride + 1;
        s->ac_val[2] = s->ac_val[1] + c_size;
    }

    return 0;
fail:
    return -1; // free() through ff_MPV_common_end()
}
