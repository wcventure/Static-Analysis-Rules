        )
{
    int output_count;
    unsigned int history = rice_initialhistory;
    int sign_modifier = 0;

    for (output_count = 0; output_count < output_size; output_count++) {
        int32_t x;
        int32_t x_modified;
        int32_t final_val;

        /
        int k; /

        /
        k = av_log2((history >> 9) + 3);
        x= decode_scalar(&alac->gb, k, rice_kmodifier, readsamplesize);

        x_modified = sign_modifier + x;
        final_val = (x_modified + 1) / 2;
        if (x_modified & 1) final_val *= -1;

        output_buffer[output_count] = final_val;

        sign_modifier = 0;

        /
        history += x_modified * rice_historymult
                   - ((history * rice_historymult) >> 9);

        if (x_modified > 0xffff)
            history = 0xffff;

        /
        if ((history < 128) && (output_count+1 < output_size)) {
            int block_size, k;

            sign_modifier = 1;

            k = 7 - av_log2(history) + ((history + 16) >> 6 /);

            block_size= decode_scalar(&alac->gb, k, rice_kmodifier, 16);

            if (block_size > 0) {
                memset(&output_buffer[output_count+1], 0, block_size * 4);
                output_count += block_size;
            }

            if (block_size > 0xffff)
                sign_modifier = 0;

            history = 0;
        }
    }
}
