static inline long double compute_read_bwidth(void)
{
    assert(block_mig_state.total_time != 0);
    return  (block_mig_state.reads * BLOCK_SIZE)/ block_mig_state.total_time;
}
