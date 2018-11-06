static int read_sl_header(PESContext *pes, SLConfigDescr *sl,
                          const uint8_t *buf, int buf_size)
{
    GetBitContext gb;
    int au_start_flag = 0, au_end_flag = 0, ocr_flag = 0, idle_flag = 0;
    int padding_flag = 0, padding_bits = 0, inst_bitrate_flag = 0;
    int dts_flag = -1, cts_flag = -1;
    int64_t dts = AV_NOPTS_VALUE, cts = AV_NOPTS_VALUE;

    init_get_bits(&gb, buf, buf_size * 8);

    if (sl->use_au_start)
        au_start_flag = get_bits1(&gb);
    if (sl->use_au_end)
        au_end_flag = get_bits1(&gb);
    if (!sl->use_au_start && !sl->use_au_end)
        au_start_flag = au_end_flag = 1;
    if (sl->ocr_len > 0)
        ocr_flag = get_bits1(&gb);
    if (sl->use_idle)
        idle_flag = get_bits1(&gb);
    if (sl->use_padding)
        padding_flag = get_bits1(&gb);
    if (padding_flag)
        padding_bits = get_bits(&gb, 3);

    if (!idle_flag && (!padding_flag || padding_bits != 0)) {
        if (sl->packet_seq_num_len)
            skip_bits_long(&gb, sl->packet_seq_num_len);
        if (sl->degr_prior_len)
            if (get_bits1(&gb))
                skip_bits(&gb, sl->degr_prior_len);
        if (ocr_flag)
            skip_bits_long(&gb, sl->ocr_len);
        if (au_start_flag) {
            if (sl->use_rand_acc_pt)
                get_bits1(&gb);
            if (sl->au_seq_num_len > 0)
                skip_bits_long(&gb, sl->au_seq_num_len);
            if (sl->use_timestamps) {
                dts_flag = get_bits1(&gb);
                cts_flag = get_bits1(&gb);
            }
        }
        if (sl->inst_bitrate_len)
            inst_bitrate_flag = get_bits1(&gb);
        if (dts_flag == 1)
            dts = get_ts64(&gb, sl->timestamp_len);
        if (cts_flag == 1)
            cts = get_ts64(&gb, sl->timestamp_len);
        if (sl->au_len > 0)
            skip_bits_long(&gb, sl->au_len);
        if (inst_bitrate_flag)
            skip_bits_long(&gb, sl->inst_bitrate_len);
    }

    if (dts != AV_NOPTS_VALUE)
        pes->dts = dts;
    if (cts != AV_NOPTS_VALUE)
        pes->pts = cts;

    if (sl->timestamp_len && sl->timestamp_res)
        avpriv_set_pts_info(pes->st, sl->timestamp_len, 1, sl->timestamp_res);

    return (get_bits_count(&gb) + 7) >> 3;
}
