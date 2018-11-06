static void svq3_luma_dc_dequant_idct_c(int16_t *output, int16_t *input, int qp)
{
    const int qmul = svq3_dequant_coeff[qp];
#define stride 16
    int i;
    int temp[16];
    static const uint8_t x_offset[4] = { 0, 1 * stride, 4 * stride, 5 * stride };

    for (i = 0; i < 4; i++) {
        const int z0 = 13 * (input[4 * i + 0] +      input[4 * i + 2]);
        const int z1 = 13 * (input[4 * i + 0] -      input[4 * i + 2]);
        const int z2 =  7 *  input[4 * i + 1] - 17 * input[4 * i + 3];
        const int z3 = 17 *  input[4 * i + 1] +  7 * input[4 * i + 3];

        temp[4 * i + 0] = z0 + z3;
        temp[4 * i + 1] = z1 + z2;
        temp[4 * i + 2] = z1 - z2;
        temp[4 * i + 3] = z0 - z3;
    }

    for (i = 0; i < 4; i++) {
        const int offset = x_offset[i];
        const int z0     = 13 * (temp[4 * 0 + i] +      temp[4 * 2 + i]);
        const int z1     = 13 * (temp[4 * 0 + i] -      temp[4 * 2 + i]);
        const int z2     =  7 *  temp[4 * 1 + i] - 17 * temp[4 * 3 + i];
        const int z3     = 17 *  temp[4 * 1 + i] +  7 * temp[4 * 3 + i];

        output[stride *  0 + offset] = (z0 + z3) * qmul + 0x80000 >> 20;
        output[stride *  2 + offset] = (z1 + z2) * qmul + 0x80000 >> 20;
        output[stride *  8 + offset] = (z1 - z2) * qmul + 0x80000 >> 20;
        output[stride * 10 + offset] = (z0 - z3) * qmul + 0x80000 >> 20;
    }
}
