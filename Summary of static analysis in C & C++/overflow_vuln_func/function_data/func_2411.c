static inline void gen_sync_flags(DisasContext *dc)
{
    /
    if (dc->tb_flags != dc->synced_flags) {
        tcg_gen_movi_tl(env_flags, dc->tb_flags);
        dc->synced_flags = dc->tb_flags;
    }
}
