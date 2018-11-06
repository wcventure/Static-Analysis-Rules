static void find_best_state(uint8_t best_state[256][256],
                            const uint8_t one_state[256])
{
    int i, j, k, m;
    double l2tab[256];

    for (i = 1; i < 256; i++)
        l2tab[i] = log2(i / 256.0);

    for (i = 0; i < 256; i++) {
        double best_len[256];
        double p = i / 256.0;

        for (j = 0; j < 256; j++)
            best_len[j] = 1 << 30;

        for (j = FFMAX(i - 10, 1); j < FFMIN(i + 11, 256); j++) {
            double occ[256] = { 0 };
            double len      = 0;
            occ[j] = 1.0;
            for (k = 0; k < 256; k++) {
                double newocc[256] = { 0 };
                for (m = 1; m < 256; m++)
                    if (occ[m]) {
                        len -= occ[m] *     (p  * l2tab[m] +
                                        (1 - p) * l2tab[256 - m]);
                    }
                if (len < best_len[k]) {
                    best_len[k]      = len;
                    best_state[i][k] = j;
                }
                for (m = 0; m < 256; m++)
                    if (occ[m]) {
                        newocc[one_state[m]]             += occ[m] * p;
                        newocc[256 - one_state[256 - m]] += occ[m] * (1 - p);
                    }
                memcpy(occ, newocc, sizeof(occ));
            }
        }
    }
}
