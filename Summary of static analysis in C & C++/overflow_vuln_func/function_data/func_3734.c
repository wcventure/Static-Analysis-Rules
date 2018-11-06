void ff_xface_generate_face(uint8_t *dst, uint8_t * const src)
{
    int h, i, j, k, l, m;

    for (j = 0; j < XFACE_HEIGHT; j++) {
        for (i = 0; i < XFACE_WIDTH; i++) {
            h = i + j * XFACE_WIDTH;
            k = 0;

            /

            for (l = i - 2; l <= i + 2; l++) {
                for (m = j - 2; m <= j; m++) {
                    if (l >= i && m == j)
                        continue;
                    if (l > 0 && l <= XFACE_WIDTH && m > 0)
                        k = 2*k + src[l + m * XFACE_WIDTH];
                }
            }

            /

#define GEN(table) dst[h] ^= (table[k>>3]>>(7-(k&7)))&1

            switch (i) {
            case 1:
                switch (j) {
                case 1:  GEN(g_22); break;
                case 2:  GEN(g_21); break;
                default: GEN(g_20); break;
                }
                break;
            case 2:
                switch (j) {
                case 1:  GEN(g_12); break;
                case 2:  GEN(g_11); break;
                default: GEN(g_10); break;
                }
                break;
            case XFACE_WIDTH - 1:
                switch (j) {
                case 1:  GEN(g_42); break;
                case 2:  GEN(g_41); break;
                default: GEN(g_40); break;
                }
                break;
            case XFACE_WIDTH:
                switch (j) {
                case 1:  GEN(g_32); break;
                case 2:  GEN(g_31); break;
                default: GEN(g_30); break;
                }
                break;
            default:
                switch (j) {
                case 1:  GEN(g_02); break;
                case 2:  GEN(g_01); break;
                default: GEN(g_00); break;
                }
                break;
            }
        }
    }
}
