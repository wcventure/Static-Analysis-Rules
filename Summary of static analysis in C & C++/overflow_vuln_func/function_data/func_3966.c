int32_t ff_mlp_pack_output(int32_t lossless_check_data,
                           uint16_t blockpos,
                           int32_t (*sample_buffer)[MAX_CHANNELS],
                           void *data,
                           uint8_t *ch_assign,
                           int8_t *output_shift,
                           uint8_t max_matrix_channel,
                           int is32)
{
    unsigned int i, out_ch = 0;
    int32_t *data_32 = data;
    int16_t *data_16 = data;

    for (i = 0; i < blockpos; i++) {
        for (out_ch = 0; out_ch <= max_matrix_channel; out_ch++) {
            int mat_ch = ch_assign[out_ch];
            int32_t sample = sample_buffer[i][mat_ch] *
                          (1U << output_shift[mat_ch]);
            lossless_check_data ^= (sample & 0xffffff) << mat_ch;
            if (is32)
                *data_32++ = sample * 256;
            else
                *data_16++ = sample >> 8;
        }
    }
    return lossless_check_data;
}
