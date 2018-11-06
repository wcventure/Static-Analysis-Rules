static void dv_decode_ac(GetBitContext *gb, BlockInfo *mb, DCTELEM *block)
{
    int last_index = gb->size_in_bits;
    const uint8_t  *scan_table   = mb->scan_table;
    const uint32_t *factor_table = mb->factor_table;
    int pos               = mb->pos;
    int partial_bit_count = mb->partial_bit_count;
    int level, run, vlc_len, index;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);

    /
    if (partial_bit_count > 0) {
        re_cache = ((unsigned)re_cache >> partial_bit_count) |
                   (mb->partial_bit_buffer << (sizeof(re_cache) * 8 - partial_bit_count));
        re_index -= partial_bit_count;
        mb->partial_bit_count = 0;
    }

    /
    for (;;) {
        av_dlog(NULL, "%2d: bits=%04x index=%d\n", pos, SHOW_UBITS(re, gb, 16),
                re_index);
        /
        index   = NEG_USR32(re_cache, TEX_VLC_BITS);
        vlc_len = dv_rl_vlc[index].len;
        if (vlc_len < 0) {
            index = NEG_USR32((unsigned)re_cache << TEX_VLC_BITS, -vlc_len) + dv_rl_vlc[index].level;
            vlc_len = TEX_VLC_BITS - vlc_len;
        }
        level = dv_rl_vlc[index].level;
        run   = dv_rl_vlc[index].run;

        /
        if (re_index + vlc_len > last_index) {
            /
            mb->partial_bit_count = last_index - re_index;
            mb->partial_bit_buffer = NEG_USR32(re_cache, mb->partial_bit_count);
            re_index = last_index;
            break;
        }
        re_index += vlc_len;

        av_dlog(NULL, "run=%d level=%d\n", run, level);
        pos += run;
        if (pos >= 64)
            break;

        level = (level * factor_table[pos] + (1 << (dv_iweight_bits - 1))) >> dv_iweight_bits;
        block[scan_table[pos]] = level;

        UPDATE_CACHE(re, gb);
    }
    CLOSE_READER(re, gb);
    mb->pos = pos;
}
