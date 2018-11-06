static inline void FUNC(idctRowCondDC)(DCTELEM *row)
{
    int a0, a1, a2, a3, b0, b1, b2, b3;

#if HAVE_FAST_64BIT
#define ROW0_MASK (0xffffLL << 48 * HAVE_BIGENDIAN)
    if (((((uint64_t *)row)[0] & ~ROW0_MASK) | ((uint64_t *)row)[1]) == 0) {
        uint64_t temp = (row[0] << DC_SHIFT) & 0xffff;
        temp += temp << 16;
        temp += temp << 32;
        ((uint64_t *)row)[0] = temp;
        ((uint64_t *)row)[1] = temp;
        return;
    }
#else
    if (!(((uint32_t*)row)[1] |
          ((uint32_t*)row)[2] |
          ((uint32_t*)row)[3] |
          row[1])) {
        uint32_t temp = (row[0] << DC_SHIFT) & 0xffff;
        temp += temp << 16;
        ((uint32_t*)row)[0]=((uint32_t*)row)[1] =
            ((uint32_t*)row)[2]=((uint32_t*)row)[3] = temp;
        return;
    }
#endif

    a0 = (W4 * row[0]) + (1 << (ROW_SHIFT - 1));
    a1 = a0;
    a2 = a0;
    a3 = a0;

    a0 += W2 * row[2];
    a1 += W6 * row[2];
    a2 -= W6 * row[2];
    a3 -= W2 * row[2];

    b0 = MUL(W1, row[1]);
    MAC(b0, W3, row[3]);
    b1 = MUL(W3, row[1]);
    MAC(b1, -W7, row[3]);
    b2 = MUL(W5, row[1]);
    MAC(b2, -W1, row[3]);
    b3 = MUL(W7, row[1]);
    MAC(b3, -W5, row[3]);

    if (AV_RN64A(row + 4)) {
        a0 +=   W4*row[4] + W6*row[6];
        a1 += - W4*row[4] - W2*row[6];
        a2 += - W4*row[4] + W2*row[6];
        a3 +=   W4*row[4] - W6*row[6];

        MAC(b0,  W5, row[5]);
        MAC(b0,  W7, row[7]);

        MAC(b1, -W1, row[5]);
        MAC(b1, -W5, row[7]);

        MAC(b2,  W7, row[5]);
        MAC(b2,  W3, row[7]);

        MAC(b3,  W3, row[5]);
        MAC(b3, -W1, row[7]);
    }

    row[0] = (a0 + b0) >> ROW_SHIFT;
    row[7] = (a0 - b0) >> ROW_SHIFT;
    row[1] = (a1 + b1) >> ROW_SHIFT;
    row[6] = (a1 - b1) >> ROW_SHIFT;
    row[2] = (a2 + b2) >> ROW_SHIFT;
    row[5] = (a2 - b2) >> ROW_SHIFT;
    row[3] = (a3 + b3) >> ROW_SHIFT;
    row[4] = (a3 - b3) >> ROW_SHIFT;
}
