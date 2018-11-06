#if CONFIG_VP7_DECODER
static void vp7_luma_dc_wht_c(int16_t block[4][4][16], int16_t dc[16])
{
    int i, a1, b1, c1, d1;
    int16_t tmp[16];

    for (i = 0; i < 4; i++) {
        a1 = (dc[i * 4 + 0] + dc[i * 4 + 2]) * 23170;
        b1 = (dc[i * 4 + 0] - dc[i * 4 + 2]) * 23170;
        c1 = dc[i * 4 + 1] * 12540 - dc[i * 4 + 3] * 30274;
        d1 = dc[i * 4 + 1] * 30274 + dc[i * 4 + 3] * 12540;
        tmp[i * 4 + 0] = (a1 + d1) >> 14;
        tmp[i * 4 + 3] = (a1 - d1) >> 14;
        tmp[i * 4 + 1] = (b1 + c1) >> 14;
        tmp[i * 4 + 2] = (b1 - c1) >> 14;
    }

    for (i = 0; i < 4; i++) {
        a1 = (tmp[i + 0] + tmp[i + 8]) * 23170;
        b1 = (tmp[i + 0] - tmp[i + 8]) * 23170;
        c1 = tmp[i + 4] * 12540 - tmp[i + 12] * 30274;
        d1 = tmp[i + 4] * 30274 + tmp[i + 12] * 12540;
        AV_ZERO64(dc + i * 4);
        block[0][i][0] = (a1 + d1 + 0x20000) >> 18;
        block[3][i][0] = (a1 - d1 + 0x20000) >> 18;
        block[1][i][0] = (b1 + c1 + 0x20000) >> 18;
        block[2][i][0] = (b1 - c1 + 0x20000) >> 18;
    }
}
